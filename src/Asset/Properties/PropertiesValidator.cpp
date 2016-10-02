#include "PropertiesValidator.hpp"
#include <algorithm>
#include <functional>
#include <unordered_set>

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
const std::unordered_map<PropertiesValidator::ErrorCode, std::string> PropertiesValidator::mErrMap =
{
    { 1, "Float argument is smaller than zero" },
    { 2, "Float argument is greater than 1.0"  },
    { 3, "Id is empty string"                  },
    { 4, "Metallic argument is not 0 or 1"     },
    { 5, "Duplicate id"                        },
    { 6, "Undefined reference"                 }
};

const std::unordered_map<PropertiesValidator::WarningCode, std::string> PropertiesValidator::mWarnMap =
{
    { 1, "Id is invalid" }
};
WARN_GUARD_OFF

// --------------------------------------------------
// Forward declarations
// --------------------------------------------------
static PropertiesValidator::Result GlobalValidateScene(const Properties::SceneFile& scene);

// --------------------------------------------------
// Header function implementations
// --------------------------------------------------
PropertiesValidator::Result& PropertiesValidator::Result::operator+=(const PropertiesValidator::Result& b)
{
    this->errors.insert(std::end(this->errors), std::begin(b.errors), std::end(b.errors));
    this->warnings.insert(std::end(this->warnings), std::begin(b.warnings), std::end(b.warnings));
    return *this;
}

PropertiesValidator::Result& PropertiesValidator::Result::operator+=(const PropertiesValidator::Result&& b)
{
    std::move(std::begin(b.errors), std::end(b.errors), std::back_inserter(this->errors));
    std::move(std::begin(b.warnings), std::end(b.warnings), std::back_inserter(this->warnings));
    return *this;
}

// Id
template<>
PropertiesValidator::Result PropertiesValidator::Validate<Properties::Id>
(const Properties::Id& input)
{
    Result r = {};
    if (!input.valid)
        r.warnings.push_back({1, mCurrentId});
    else if(input.data == "")
        r.errors.push_back({3, mCurrentId});
    return r;
}

// Color
template<>
PropertiesValidator::Result PropertiesValidator::Validate<Properties::Color>
(const Properties::Color& input)
{
    (void)input;
    Result r = {};
    return r;
}

// Geometry
template<>
PropertiesValidator::Result PropertiesValidator::Validate<Properties::Geometry>
(const Properties::Geometry& input)
{
    Result r = {};
    return r += Validate(input.id);
}

// Texture
template<>
PropertiesValidator::Result PropertiesValidator::Validate<Properties::Texture>
(const Properties::Texture& input)
{
    Result r = {};
    return r += Validate(input.id);
}

// Material
template<>
PropertiesValidator::Result PropertiesValidator::Validate<Properties::Material>
(const Properties::Material& input)
{
    Result r = {};

    // Validate ID
    r += Validate(input.id);

    // Validate Colors
    r += Validate(input.color);
    r += Validate(input.emissive);

    // Validate attributes
    for (float a : { input.roughness, input.reflectivity, input.transparency })
    {
        if (a < 0)
            r.errors.push_back({1, mCurrentId});
        else if (a > 1)
            r.errors.push_back({2, mCurrentId});
    }

    if (input.metallic != 0 && input.metallic != 1)
        r.errors.push_back({4, mCurrentId});

    return r;
}

// Transform
template<>
PropertiesValidator::Result PropertiesValidator::Validate<Properties::Transform>
(const Properties::Transform& input)
{
    (void)input;
    Result r = {};
    return r;
}

// Model
template<>
PropertiesValidator::Result PropertiesValidator::Validate<Properties::Model>
(const Properties::Model& input)
{
    Result r = {};

    // Validate IDs
    r += Validate(input.id);
    r += Validate(input.geometry);
    for (const auto& id : input.materials)
        r += Validate(id);

    return r;
}

// Scene Node
template<>
PropertiesValidator::Result PropertiesValidator::Validate<Properties::SceneNode>
(const Properties::SceneNode& input)
{
    Result r = {};

    // Id
    r += Validate(input.id);

    // Model Id
    r += Validate(input.model);

    // Transform
    r += Validate(input.transform);

    // Children
    for (const auto& child : input.children)
        r += Validate(child);

    return r;
}

// Scene
template<>
PropertiesValidator::Result PropertiesValidator::Validate<Properties::Scene>
(const Properties::Scene& input)
{
    Result r = {};

    for (const auto& n : input.nodes)
        r += Validate(n);

    return r;
}

// Material File
template<>
PropertiesValidator::Result PropertiesValidator::Validate<Properties::MaterialFile>
(const Properties::MaterialFile& input)
{
    PropertiesValidator::Result r = {};

    // Validate textures
    for (const auto& t : input.textures)
        r += Validate(t);

    // Validate materials
    for (const auto& m : input.materials)
        r += Validate(m);

    return r;
}

// Model File
template<>
PropertiesValidator::Result PropertiesValidator::Validate<Properties::ModelFile>
(const Properties::ModelFile& input)
{
    PropertiesValidator::Result r = {};

    // Validate textures
    for (const auto& g : input.geometries)
        r += Validate(g);

    // Validate models
    for (const auto& m : input.models)
        r += Validate(m);

    return r;
}

// Scene File
template <>
PropertiesValidator::Result PropertiesValidator::Validate<Properties::SceneFile>
(const Properties::SceneFile& input)
{
    PropertiesValidator::Result r = {};

    // Validate extra materials
    r += Validate(input.extraMaterials);

    // Validate extra models
    r += Validate(input.extraModels);

    // Validate scene
    r += Validate(input.scene);

    return r;
}

// Global SceneFile Validation
PropertiesValidator::Result PropertiesValidator::ValidateGlobal(const Properties::SceneFile& scene) const
{
    return GlobalValidateScene(scene);
}

std::string PropertiesValidator::ErrorToString(ErrorCode err)
{
    const auto it = mErrMap.find(err);
    return it != std::end(mErrMap) ? it->second : "";
}

std::string PropertiesValidator::WarnToString(WarningCode warn)
{
    const auto it = mWarnMap.find(warn);
    return it != std::end(mWarnMap) ? it->second : "";
}

// --------------------------------------------------
//  Global Validation helpers
// --------------------------------------------------

//
// Helpers for unordered_set
//
static std::size_t CustomHash(const std::string& s1, const std::string& s2)
{
        std::size_t h1 = std::hash<std::string>{}(s1);
        std::size_t h2 = std::hash<std::string>{}(s2);
        return h1 ^ (h2 << 1);
}

namespace P = Properties;
// Hash structs
struct TexHash {std::size_t operator()(const P::Texture& t)   const {return CustomHash(t.id.data, t.url);}};
struct MatHash {std::size_t operator()(const P::Material& m)  const {return CustomHash(m.id.data, m.name);}};
struct GeoHash {std::size_t operator()(const P::Geometry& g)  const {return CustomHash(g.id.data, g.url);}};
struct ModHash {std::size_t operator()(const P::Model& m)     const {return CustomHash(m.id.data, m.name);}};
struct NodHash {std::size_t operator()(const P::SceneNode& n) const {return CustomHash(n.id.data, n.model.data);}};

// Compare structs
struct TexComp {bool operator()(const P::Texture& a,   const P::Texture& b)   const {return  a.id.data == b.id.data;}};
struct MatComp {bool operator()(const P::Material& a,  const P::Material& b)  const {return  a.id.data == b.id.data;}};
struct GeoComp {bool operator()(const P::Geometry& a,  const P::Geometry& b)  const {return  a.id.data == b.id.data;}};
struct ModComp {bool operator()(const P::Model& a,     const P::Model& b)     const {return  a.id.data == b.id.data;}};
struct NodComp {bool operator()(const P::SceneNode& a, const P::SceneNode& b) const {return  a.id.data == b.id.data;}};

template <typename T, typename H, typename C>
static PropertiesValidator::Result DuplicateIdCheck(
    std::unordered_set<T, H, C>& set,
    const std::vector<T>& input)
{
    PropertiesValidator::Result r = {};

    for (const auto& v : input)
    {
        auto result = set.insert(v);
        if (!result.second)
           r.errors.push_back({5, v.id.data});
    }

    return r;
}

template <typename T, typename H, typename C>
static PropertiesValidator::Result UndefinedRefCheck(
    const std::unordered_set<T, H, C>& valueSet,
    const Properties::Id& id)
{
    PropertiesValidator::Result r = {};

    const auto it = std::find_if(
        std::begin(valueSet),
        std::end(valueSet),
        [&id](const T& v) -> bool { return id.data == v.id.data || id.data == ""; });

    if (it == std::end(valueSet))
        r.errors.push_back({6, id.data});

    return r;
}

static PropertiesValidator::Result GlobalValidateScene(const Properties::SceneFile& scene)
{
    // Avoid unecessary use of namespace for verbosity
    using Properties::Texture;
    using Properties::Material;
    using Properties::Geometry;
    using Properties::Model;
    using Properties::SceneNode;
    using Properties::Scene;
    using Properties::MaterialFile;
    using Properties::ModelFile;
    using Properties::SceneFile;

    PropertiesValidator::Result r = {};

    // Gather all IDs
    std::unordered_set<Texture, TexHash, TexComp>   textures;
    std::unordered_set<Material, MatHash, MatComp>  materials;
    std::unordered_set<Geometry, GeoHash, GeoComp>  geometries;
    std::unordered_set<Model, ModHash, ModComp>     models;
    std::unordered_set<SceneNode, NodHash, NodComp> sceneNodes;

    //
    // Multiple Definitions check
    //
    // Add values to set and take note of errors
    r += DuplicateIdCheck(textures,   scene.extraMaterials.textures);
    r += DuplicateIdCheck(materials,  scene.extraMaterials.materials);
    r += DuplicateIdCheck(geometries, scene.extraModels.geometries);
    r += DuplicateIdCheck(models,     scene.extraModels.models);

    // Check scene nodes and their children
    std::function<void(const std::vector<SceneNode>&)> checkNodeHelper =
        [&checkNodeHelper, &sceneNodes, &r](const std::vector<SceneNode>& nodes) -> void
        {
            // Check this node
            r += DuplicateIdCheck(sceneNodes, nodes);

            // Check children
            for (const auto& n : nodes)
                checkNodeHelper(n.children);
        };

    checkNodeHelper(scene.scene.nodes);

    //
    // Undefined ID Reference Check
    //
    // Check materials for undefined texture IDs
    for (const auto& m : materials)
    {
        r += UndefinedRefCheck(textures, m.dmap);
        r += UndefinedRefCheck(textures, m.smap);
        r += UndefinedRefCheck(textures, m.nmap);
    }

    // Check models for undefined materials and geometries
    for (const auto& m : models)
    {
        r += UndefinedRefCheck(geometries, m.geometry);
        for (const auto& mat : m.materials)
            r += UndefinedRefCheck(materials, mat);
    }

    // Check nodes for undefined models
    std::function<void(const std::vector<SceneNode>&)> undefRefNodeHelper =
        [&undefRefNodeHelper, &models, &r](const std::vector<SceneNode>& nodes) -> void
        {
            for (const auto& n : nodes)
            {
                // Check this node
                r += UndefinedRefCheck(models, n.model);

                // Check children
                undefRefNodeHelper(n.children);
            }
        };

    undefRefNodeHelper(scene.scene.nodes);

    return r;
}
