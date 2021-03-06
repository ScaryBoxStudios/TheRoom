#include "PropertiesLoader.hpp"
#include <assert.h>
#include <cstring>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "Properties.hpp"

// Parsing helper functions declarations
Properties::Id ParseId(const rapidjson::Value& v);          // Id 
Properties::Color ParseColor(const rapidjson::Value& v);    // Color
Properties::Transform ParseTransform(rapidjson::Value& t);  // Transform
Properties::SceneNode ParseSceneNode(rapidjson::Value& sn); // SceneNode

template <>
Properties::MaterialFile PropertiesLoader::Load<Properties::MaterialFile>(rapidjson::Document& doc)
{
    // Less verbose usage
    using namespace rapidjson;

    // Create the output material file struct
    Properties::MaterialFile matFile;

    // Parse textures
    if (doc.HasMember("textures"))
    {
        Value& textures = doc["textures"];
        assert(textures.IsArray());

        for (SizeType i = 0; i < textures.Size(); ++i)
        {
            // Parse texture
            Value& t = textures[i];
            Properties::Texture tex = {};

            // Id
            tex.id = ParseId(t["id"]);

            // Url
            tex.url = t["url"].GetString();

            // Add texture to list
            matFile.textures.push_back(tex);
        }
    }

    // Parse materials
    if (doc.HasMember("materials"))
    {
        Value& materials = doc["materials"];
        assert(materials.IsArray());

        for (SizeType i = 0; i < materials.Size(); ++i)
        {
            // Parse maetrial
            Value& m = materials[i];
            Properties::Material mt = {};

            // Id
            mt.id = ParseId(m["id"]);

            // Name
            if(m.HasMember("name"))
                mt.name = m["name"].GetString();

            // Colors
            if(m.HasMember("color"))
                mt.color = ParseColor(m["color"]);
            if (m.HasMember("emissive"))
                mt.emissive = ParseColor(m["emissive"]);

            // Aspects
            if (m.HasMember("roughness"))
                mt.roughness = static_cast<float>(m["roughness"].GetDouble());
            if (m.HasMember("reflectivity"))
                mt.reflectivity = static_cast<float>(m["reflectivity"].GetDouble());
            if (m.HasMember("metallic"))
                mt.metallic = static_cast<float>(m["metallic"].GetDouble());

            // Opacity
            if (m.HasMember("transparency"))
                mt.transparency = static_cast<float>(m["transparency"].GetDouble());

            // Wireframe
            if (m.HasMember("wireframe"))
                mt.wireframe = m["wireframe"].GetBool();

            // Map
            mt.dmap.valid = false;
            if (m.HasMember("dmap"))
                mt.dmap = ParseId(m["dmap"]);

            // SpecMap
            mt.smap.valid = false;
            if (m.HasMember("smap"))
                mt.smap = ParseId(m["smap"]);

            // Nmap
            mt.nmap.valid = false;
            if(m.HasMember("nmap"))
                mt.nmap = ParseId(m["nmap"]);

            // Add parsed material to the list
            matFile.materials.push_back(mt);
        }
    }

    return matFile;
}

template<>
Properties::ModelFile PropertiesLoader::Load<Properties::ModelFile>(rapidjson::Document& doc)
{
    // Less verbose usage
    using namespace rapidjson;

    // Create the output model file struct
    Properties::ModelFile modelFile = {};

    // Parse geometries  
    if (doc.HasMember("geometries"))
    {
        Value& geometries = doc["geometries"];
        assert(geometries.IsArray());

        for (SizeType i = 0; i < geometries.Size(); ++i)
        {
            // Parse geometry
            Value& g = geometries[i];
            Properties::Geometry gd = {};

            // Id
            gd.id = ParseId(g["id"]);

            // Name 
            gd.name = g["name"].GetString();

            // Url
            gd.url = g["url"].GetString();

            // Add parsed geometry to the list
            modelFile.geometries.push_back(gd);
        }
    }
 
    // Parse models
    if (doc.HasMember("models"))
    {
        Value& models = doc["models"];
        assert(models.IsArray());

        for (SizeType i = 0; i < models.Size(); ++i)
        {
            // Parse model
            Value& m = models[i];
            Properties::Model model = {};

            // Id
            model.id.valid = false;
            model.id = ParseId(m["id"]);

            // Geometry Id
            model.geometry.valid = false;
            model.geometry = ParseId(m["geometry"]);

            // Name
            model.name = m["name"].GetString();

            // Material
            if (m.HasMember("materials"))
            {
                Value& mats = m["materials"];
                assert(mats.IsArray());

                model.materials.resize(mats.Size());
                for (SizeType j = 0; j < mats.Size(); ++j)
                {
                    Value& mat = mats[j];
                    model.materials[j] = ParseId(mat);
                }
            }

            // Add to list
            modelFile.models.push_back(model);
        }
    }

    return modelFile;
}

void GetNestedDocument(rapidjson::Document& doc, rapidjson::Document& result, const char* key)
{
    rapidjson::StringBuffer buffer;
    assert(doc[key].IsObject());

    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc[key].Accept(writer);

    rapidjson::StringStream s(buffer.GetString());
    result.ParseStream(s);
}

template<>
Properties::SceneFile PropertiesLoader::Load<Properties::SceneFile>(rapidjson::Document& doc)
{
    // Less verbose usage
    using namespace rapidjson;

    // Create the output model file struct
    Properties::SceneFile sceneFile = {};

    // Parse extra Materials
    if (doc.HasMember("extraMaterials"))
    {
        Document subDoc;
        GetNestedDocument(doc, subDoc, "extraMaterials");
        sceneFile.extraMaterials = Load<Properties::MaterialFile>(subDoc);
    }

    // Parse extra models
    if (doc.HasMember("extraModels"))
    {
        Document subDoc;
        GetNestedDocument(doc, subDoc, "extraModels");
        sceneFile.extraModels = Load<Properties::ModelFile>(subDoc);
    }

    // Parse scene
    if (doc.HasMember("scene"))
    {
        Value& sceneVal = doc["scene"];
        assert(sceneVal.IsObject());

        // Parse scene nodes
        Value& nodes = sceneVal["nodes"];
        assert(nodes.IsArray());

        for (SizeType i = 0; i < nodes.Size(); ++i)
            sceneFile.scene.nodes.push_back(ParseSceneNode(nodes[i]));
    }

    return sceneFile;
}

// --------------------------------------------------
// Parsing helper functions implementations
// --------------------------------------------------
// Id Parse helper
Properties::Id ParseId(const rapidjson::Value& v) { return Properties::Id { v.GetString(), true }; }

// Color Parse helper
Properties::Color ParseColor(const rapidjson::Value& v)
{
    Properties::Color col = {};
    assert(v.IsArray());
    col.r = static_cast<std::uint8_t>(v[0].GetInt());
    col.g = static_cast<std::uint8_t>(v[1].GetInt());
    col.b = static_cast<std::uint8_t>(v[2].GetInt());
    col.a = static_cast<std::uint8_t>(v[3].GetInt());
    return col;
};

// Transform Parse helper
Properties::Transform ParseTransform(rapidjson::Value& t)
{
    using namespace rapidjson;
    Properties::Transform transform = {};
    transform.scale = glm::vec3(1.0f);

    // Position
    if (t.HasMember("position"))
    {
        Value& pos = t["position"];
        assert(pos.IsArray());
        transform.position.x = static_cast<float>(pos[0].GetDouble());
        transform.position.y = static_cast<float>(pos[1].GetDouble());
        transform.position.z = static_cast<float>(pos[2].GetDouble());
    }

    // Scale
    if (t.HasMember("scale"))
    {
        Value& sc = t["scale"];
        assert(sc.IsArray());
        transform.scale.x = static_cast<float>(sc[0].GetDouble());
        transform.scale.y = static_cast<float>(sc[1].GetDouble());
        transform.scale.z = static_cast<float>(sc[2].GetDouble());
    }

    // Rotation
    if (t.HasMember("rotation"))
    {
        Value& rot = t["rotation"];
        transform.rotation.x = static_cast<float>(rot[0].GetDouble());
        transform.rotation.y = static_cast<float>(rot[1].GetDouble());
        transform.rotation.z = static_cast<float>(rot[2].GetDouble());
    }
    return transform;
};

// Scene Node Parse helper
Properties::SceneNode ParseSceneNode(rapidjson::Value& sn)
{
    using namespace rapidjson;
    Properties::SceneNode sceneNode = {};
    assert(sn.IsObject());

    // Id
    sceneNode.id = ParseId(sn["id"]);

    // Model Id
    sceneNode.model = ParseId(sn["model"]);

    // Type
    std::string type = sn["type"].GetString();
    if (type == "Model")
        sceneNode.type = Properties::SceneNode::Type::Model;
    else if (type == "PointLight")
        sceneNode.type = Properties::SceneNode::Type::PointLight;
    else
        assert(false);

    // Transform
    sceneNode.transform = ParseTransform(sn["transform"]);

    // Children
    if (sn.HasMember("children"))
    {
        Value& children = sn["children"];
        assert(children.IsArray());

        for (SizeType i = 0; i < children.Size(); ++i)
            sceneNode.children.push_back(ParseSceneNode(children[i]));
    }

    return sceneNode;
};

void ParseJson(const PropertiesLoader::Buffer& data, rapidjson::Document& doc)
{
    // Less verbose usage
    using namespace rapidjson;

    // Convert it to std::string containter
    std::string json(std::begin(data), std::end(data));

    // The scene file object to be filled
    Properties::SceneFile sc = {};

    // Parse the json file
    doc.Parse(json.c_str());

    // Root
    assert(doc.IsObject());
}
