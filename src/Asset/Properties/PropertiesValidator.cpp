#include "PropertiesValidator.hpp"
#include "Properties.hpp"

#include "../../Util/WarnGuard.hpp"
WARN_GUARD_ON
const std::unordered_map<PropertiesValidator::ErrorCode, std::string> PropertiesValidator::mErrMap =
{
    { 1, "Float argument is smaller than zero" },
    { 2, "Float argument is greater than 1.0"  },
    { 3, "Id is empty string"                  },
    { 4, "Metallic argument is not 0 or 1"     }
};

const std::unordered_map<PropertiesValidator::WarningCode, std::string> PropertiesValidator::mWarnMap =
{
    { 1, "Id is invalid" }
};
WARN_GUARD_OFF

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
