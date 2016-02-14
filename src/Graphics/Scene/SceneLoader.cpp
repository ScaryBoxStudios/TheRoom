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

            // UUID
            assert(StringToUUID(g["uuid"].GetString(), gd.uuid));

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

            // UUID
            assert(StringToUUID(im["uuid"].GetString(), img.uuid));

            // Url
            img.url = im["url"].GetString();

            // Add parsed image to the list
            sc.images.push_back(img);
        }
    }

    // Helper
    auto parseColor = [](std::uint32_t v) -> SceneFile::Color
    {
        SceneFile::Color col = {};
        col.r = (v >> 0) & 0xFF;
        col.g = (v >> 8) & 0xFF;
        col.b = (v >> 16) & 0xFF;
        col.a = (v >> 24) & 0xFF;
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

            // UUID
            assert(StringToUUID(t["uuid"].GetString(), tex.uuid));

            // Image
            assert(StringToUUID(t["image"].GetString(), tex.image));

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
                tex.borderColor = parseColor(t["borderColor"].GetInt());

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

            // UUID
            assert(StringToUUID(m["uuid"].GetString(), mt.uuid));

            // Name
            if(m.HasMember("name"))
                mt.name = m["name"].GetString();

            // Type
            mt.type = m["type"].GetString();

            // Colors
            if(m.HasMember("color"))
                mt.color = parseColor(m["color"].GetInt());
            if (m.HasMember("ambient"))
                mt.ambient = parseColor(m["ambient"].GetInt());
            if (m.HasMember("emissive"))
                mt.emissive = parseColor(m["emissive"].GetInt());
            if (m.HasMember("specular"))
                mt.specular = parseColor(m["specular"].GetInt());
            if (m.HasMember("shininess"))
                mt.shininess = static_cast<float>(m["shininess"].GetDouble());

            // Opacity
            if (m.HasMember("opacity"))
                mt.opacity = static_cast<float>(m["opacity"].GetDouble());

            // Transparent
            if (m.HasMember("transparent"))
                mt.transparent = m["transparent"].GetBool();

            // Wireframe
            if (m.HasMember("wireframe"))
                mt.wireframe = m["wireframe"].GetBool();

            // Map
            if (m.HasMember("map"))
                if(strcmp(m["map"].GetString(), "") == 0)
                    mt.map.SetUuid("");
                else
                    assert(StringToUUID(m["map"].GetString(), mt.map));

            // SpecMap
            if (m.HasMember("specMap"))
                if(strcmp(m["specMap"].GetString(), "") == 0)
                    mt.specMap.SetUuid("");
                else
                    assert(StringToUUID(m["specMap"].GetString(), mt.specMap));

            // Nmap
            if(m.HasMember("nmap"))
                if(strcmp(m["nmap"].GetString(), "") == 0)
                    mt.nmap.SetUuid("");
                else
                    assert(StringToUUID(m["nmap"].GetString(), mt.nmap));

            // Add parsed material to the list
            sc.materials.push_back(mt);
        }
    }

    // Parse root object
    if(doc.HasMember("object"))
    {
        Value& object = doc["object"];

        // UUID
        assert(StringToUUID(object["uuid"].GetString(), sc.object.uuid));

        // Type
        sc.object.type = object["type"].GetString();

        // Helpers
        auto parseMatrix = [](std::vector<float> v) -> glm::mat4
        {
            glm::mat4 mat;
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    mat[i][j] = v[i * 4 + j];
            return mat;
        };
        auto arrayToVector = [](Value& a) -> std::vector<float>
        {
            assert(a.IsArray());
            std::vector<float> v;
            for (SizeType i = 0; i < a.Size(); ++i)
                v.push_back(static_cast<float>(a[i].GetDouble()));
            return v;
        };

        // Matrix
        sc.object.matrix = parseMatrix(arrayToVector(object["matrix"]));

        // Children
        Value& children = object["children"];
        assert(children.IsArray());

        for (SizeType i = 0; i < children.Size(); ++i)
        {
            Value& child = children[i];
            SceneFile::Object::Child ch;

            // UUID
            assert(StringToUUID(child["uuid"].GetString(), ch.uuid));

            // Type
            ch.type = child["type"].GetString();

            // Name
            ch.name = child["name"].GetString();

            // Matrix
            ch.matrix = parseMatrix(arrayToVector(child["matrix"]));

            // Geometry
            if (child.HasMember("geometry"))
                assert(StringToUUID(child["geometry"].GetString(), ch.geometry));

            // Material
            if (child.HasMember("material"))
                assert(StringToUUID(child["material"].GetString(), ch.material));

            // Add to list
            sc.object.children.push_back(ch);
        }
    }

    return sc;
}
