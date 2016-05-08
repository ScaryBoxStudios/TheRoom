#include "SceneLoader.hpp"
#include <assert.h>
#include <cstring>
WARN_GUARD_ON
#include <rapidjson/document.h>
WARN_GUARD_OFF

SceneFile SceneLoader::Load(const Buffer& data)
{
    // Less verbose usage
    using namespace rapidjson;

    // Convert it to std::string containter
    std::string json(std::begin(data), std::end(data));

    // The scene file object to be filled
    SceneFile sc = {};

    // Parse the json file
    Document doc;
    doc.Parse(json.c_str());

    // Root
    assert(doc.IsObject());

    // Id parse helper
    auto parseId = [](Value& v) -> SceneFile::Id { return SceneFile::Id { v.GetUint(), true }; };

    // Parse metadata
    if(doc.HasMember("metadata"))
    {
        Value& metadata = doc["metadata"];
        assert(metadata.IsObject());

        // Version
        if(metadata.HasMember("version"))
        {
            Value& version = metadata["version"];
            sc.metadata.version = std::to_string(version.GetDouble());
        }

        // Type
        if(metadata.HasMember("type"))
        {
            Value& type = metadata["type"];
            sc.metadata.type = type.GetString();
        }

        // Generator
        if(metadata.HasMember("generator"))
        {
            Value& generator = metadata["generator"];
            sc.metadata.generator = generator.GetString();
        }
    }

    // Parse geometries
    if(doc.HasMember("geometries"))
    {
        Value& geometries = doc["geometries"];
        assert(geometries.IsArray());

        for(SizeType i = 0; i < geometries.Size(); ++i)
        {
            // Parse geometry
            Value& g = geometries[i];
            SceneFile::Geometry gd = {};

            // Id
            gd.id = parseId(g["id"]);

            // Type
            gd.type = g["type"].GetString();

            // Url
            gd.url = g["url"].GetString();

            // Add parsed geometry to the list
            sc.geometries.push_back(gd);
        }
    }

    // Parse images
    if(doc.HasMember("images"))
    {
        Value& images = doc["images"];
        assert(images.IsArray());

        for(SizeType i = 0; i < images.Size(); ++i)
        {
            // Parse image
            Value& im = images[i];
            SceneFile::Image img = {};

            // Id
            img.id = parseId(im["id"]);

            // Url
            img.url = im["url"].GetString();

            // Add parsed image to the list
            sc.images.push_back(img);
        }
    }

    // Helper
    auto parseColor = [](const Value& pos) -> SceneFile::Color
    {
        SceneFile::Color col = {};
        assert(pos.IsArray());
        col.r = static_cast<std::uint8_t>(pos[0].GetInt());
        col.g = static_cast<std::uint8_t>(pos[1].GetInt());
        col.b = static_cast<std::uint8_t>(pos[2].GetInt());
        col.a = static_cast<std::uint8_t>(pos[3].GetInt());
        return col;
    };

    // Parse textures
    if(doc.HasMember("textures"))
    {
        Value& textures = doc["textures"];
        assert(textures.IsArray());

        for(SizeType i = 0; i < textures.Size(); ++i)
        {
            // Parse texture
            Value& t = textures[i];
            SceneFile::Texture tex = {};

            // Id
            tex.id = parseId(t["id"]);

            // Image
            tex.image = parseId(t["image"]);

            // WrapMode
            if(t.HasMember("wrap"))
            {
                Value& wrap = t["wrap"];
                assert(wrap.IsArray());
                for(SizeType j = 0; j < wrap.Size(); ++j)
                {
                    std::string curWrap = wrap[j].GetString();
                    SceneFile::Texture::WrapMode wm;
                    if (curWrap == "clampToEdge")
                        wm = SceneFile::Texture::WrapMode::ClampToEdge;
                    else if (curWrap == "clampToBorder")
                        wm = SceneFile::Texture::WrapMode::ClampToBorder;
                    else if (curWrap == "mirroredRepeat")
                        wm = SceneFile::Texture::WrapMode::MirroredRepeat;
                    else if (curWrap == "repeat")
                        wm = SceneFile::Texture::WrapMode::Repeat;
                    tex.wrap.push_back(wm);
                }
            }

            // BorderColor
            if(t.HasMember("borderColor"))
                tex.borderColor = parseColor(t["borderColor"]);

            // Add parsed texture to the list
            sc.textures.push_back(tex);
        }
    }

    // Parse materials
    if(doc.HasMember("materials"))
    {
        Value& materials = doc["materials"];
        assert(materials.IsArray());

        for (SizeType i = 0; i < materials.Size(); ++i)
        {
            // Parse material
            Value& m = materials[i];
            SceneFile::Material mt = {};

            // Id
            mt.id = parseId(m["id"]);

            // Name
            if(m.HasMember("name"))
                mt.name = m["name"].GetString();

            // Type
            mt.type = m["type"].GetString();

            // Colors
            if(m.HasMember("color"))
                mt.color = parseColor(m["color"]);
            if (m.HasMember("ambient"))
                mt.ambient = parseColor(m["ambient"]);
            if (m.HasMember("emissive"))
                mt.emissive = parseColor(m["emissive"]);
            if (m.HasMember("specular"))
                mt.specular = parseColor(m["specular"]);
            if (m.HasMember("roughness"))
                mt.roughness = static_cast<float>(m["roughness"].GetDouble());
            if (m.HasMember("fresnel"))
                mt.fresnel = static_cast<float>(m["fresnel"].GetDouble());
            if (m.HasMember("metallic"))
                mt.metallic = static_cast<float>(m["metallic"].GetDouble());

            // Opacity
            if (m.HasMember("transparency"))
                mt.transparency = static_cast<float>(m["transparency"].GetDouble());

            // Transparent
            if (m.HasMember("transparent"))
                mt.transparent = m["transparent"].GetBool();

            // Wireframe
            if (m.HasMember("wireframe"))
                mt.wireframe = m["wireframe"].GetBool();

            // Map
            mt.map.valid = false;
            if (m.HasMember("map"))
                mt.map = parseId(m["map"]);

            // SpecMap
            mt.specMap.valid = false;
            if (m.HasMember("specMap"))
                mt.specMap = parseId(m["specMap"]);

            // Nmap
            mt.nmap.valid = false;
            if(m.HasMember("nmap"))
                mt.nmap = parseId(m["nmap"]);

            // Add parsed material to the list
            sc.materials.push_back(mt);
        }
    }

    // Parse root object
    if(doc.HasMember("object"))
    {
        Value& object = doc["object"];

        // Id
        sc.object.id = parseId(object["id"]);

        // Type
        sc.object.type = object["type"].GetString();

        // Helpers
        auto parseTransform = [](Value& t) -> SceneFile::Object::Transform
        {
            SceneFile::Object::Transform transform = {};
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

        // Transform
        sc.object.transform = parseTransform(object["transform"]);

        // Children
        Value& children = object["children"];
        assert(children.IsArray());

        for (SizeType i = 0; i < children.Size(); ++i)
        {
            Value& child = children[i];
            SceneFile::Object::Child ch;

            // Id
            ch.id = parseId(child["id"]);

            // Type
            ch.type = child["type"].GetString();

            // Name
            ch.name = child["name"].GetString();

            // Matrix
            ch.transform = parseTransform(child["transform"]);

            // Geometry
            if (child.HasMember("geometry"))
                ch.geometry = parseId(child["geometry"]);

            // Material
            if (child.HasMember("materials"))
            {
                Value& mats = child["materials"];
                assert(mats.IsArray());

                ch.materials.resize(mats.Size());
                for (SizeType j = 0; j < mats.Size(); ++j)
                {
                    Value& mat = mats[j];
                    ch.materials[j] = parseId(mat);
                }
            }

            // Add to list
            sc.object.children.push_back(ch);
        }
    }

    return sc;
}
