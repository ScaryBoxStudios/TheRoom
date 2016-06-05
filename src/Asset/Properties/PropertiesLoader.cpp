#include "PropertiesLoader.hpp"
#include <assert.h>
#include <cstring>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

// Id Parse helper
auto parseId = [](rapidjson::Value& v) -> Properties::Id { return Properties::Id { v.GetString(), true }; };

// Color Parse helper
auto parseColor = [](const rapidjson::Value& pos) -> Properties::Color
{
    Properties::Color col = {};
    assert(pos.IsArray());
    col.r = static_cast<std::uint8_t>(pos[0].GetInt());
    col.g = static_cast<std::uint8_t>(pos[1].GetInt());
    col.b = static_cast<std::uint8_t>(pos[2].GetInt());
    col.a = static_cast<std::uint8_t>(pos[3].GetInt());
    return col;
};

void parseJson(const PropertiesLoader::Buffer& data, rapidjson::Document& doc)
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

// Parse Transform 
auto parseTransform = [](rapidjson::Value& t) -> Properties::Transform
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
            tex.id = parseId(t["id"]);

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
            mt.id = parseId(m["id"]);

            // Name
            if(m.HasMember("name"))
                mt.name = m["name"].GetString();

            // Colors
            if(m.HasMember("color"))
                mt.color = parseColor(m["color"]);
            if (m.HasMember("emissive"))
                mt.emissive = parseColor(m["emissive"]);

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
                mt.dmap = parseId(m["dmap"]);

            // SpecMap
            mt.smap.valid = false;
            if (m.HasMember("smap"))
                mt.smap = parseId(m["smap"]);

            // Nmap
            mt.nmap.valid = false;
            if(m.HasMember("nmap"))
                mt.nmap = parseId(m["nmap"]);

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

    // Parse group id
    if (doc.HasMember("groupId"))
        modelFile.id = parseId(doc["groupId"]);

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
            gd.id = parseId(g["id"]);

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
            model.id = parseId(m["id"]);

            // Geometry Id
            model.geometry.valid = false;
            model.geometry = parseId(m["geometry"]);

            // Name
            model.name = m["name"].GetString();

            // Transform
            model.transform = parseTransform(m["transform"]);

            // Material
            if (m.HasMember("materials"))
            {
                Value& mats = m["materials"];
                assert(mats.IsArray());

                model.materials.resize(mats.Size());
                for (SizeType j = 0; j < mats.Size(); ++j)
                {
                    Value& mat = mats[j];
                    model.materials[j] = parseId(mat);
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

        // Parse models
        if (sceneVal.HasMember("models"))
        {
            Value& models = sceneVal["models"];
            assert(models.IsArray());

            for (SizeType i = 0; i < models.Size(); ++i)
                sceneFile.scene.models.push_back(parseId(models[i]));
        }

        // Parse model groups
        if (sceneVal.HasMember("modelGroups"))
        {
            Value& groups = sceneVal["modelGroups"];
            assert(groups.IsArray());

            for (SizeType i = 0; i < groups.Size(); ++i)
                sceneFile.scene.models.push_back(parseId(groups[i]));
        }

        // Parse point lights
        if (sceneVal.HasMember("pointLights"))
        {
            Value& lights = sceneVal["pointLights"];
            assert(lights.IsArray());

            for (SizeType i = 0; i < lights.Size(); ++i)
                sceneFile.scene.models.push_back(parseId(lights[i]));
        }
    }

    return sceneFile;
}
