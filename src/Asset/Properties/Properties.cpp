#include "Properties.hpp"
#include <iostream>
#include <string>

namespace Properties
{
    void printStr(const std::string& title, const std::string& value)
    {
        std::cout << title + ": " + value << std::endl;
    }

    void printTitle(const std::string& title)
    {
        std::cout << "--- " + title + " ---" << std::endl;
    }

    template<>
    void print<Id>(const Id& id, const std::string& name)
    {
        printStr(name + " Id", id.valid ? id.data : "invalid");
    }

    template<>
    void print<Id>(const Id& id)
    {
        print(id, "");
    }

    template<>
    void print<Color>(const Color& v, const std::string& name)
    {
        printStr(name + " Color",
            std::to_string(v.r) + " " + std::to_string(v.g) + " " +
            std::to_string(v.g) + " " + std::to_string(v.a));
    }

    template<>
    void print<Geometry>(const Geometry& g)
    {
        print(g.id);
        printStr("Name", g.name);
        printStr("Url", g.url);    
    }

    template<>
    void print<Texture>(const Texture& t)
    {
        print(t.id);
        printStr("Url", t.url);
    }

    template<>
    void print<Material>(const Material& m)
    {
        printTitle("Printing Material");
        print(m.id, "Material");
        print(m.dmap, "Diffuse");
        print(m.smap, "Specular");
        print(m.nmap, "Normal");
        printStr("Name", m.name);
        print(m.color, "Material");
        print(m.emissive, "Emissive");
        printStr("Roughness", std::to_string(m.roughness));
        printStr("Reflectivity", std::to_string(m.reflectivity));
        printStr("Metallic", std::to_string(m.metallic));
        printStr("Transparency", std::to_string(m.transparency));
        printStr("Wireframe", std::to_string(m.wireframe));
    }

    template<>
    void print<Model>(const Model& m)
    {
        printTitle("Printing Model");
        print(m.id, "Model");
        print(m.id, "Geometry");
        printStr("Name", m.name);
        std::cout << "--- Model Materials ---" << std::endl;
        for (const Id& i : m.materials)
            print(i, "Mat");
    }

    template<>
    void print<ModelGroup>(const ModelGroup& m)
    {
        printTitle("Printing ModelGroup");
        print(m.id, "Group");
        for (const auto& i : m.models)
            print(i);
    }

    template<>
    void print<Scene>(const Scene& s)
    {
        printTitle("Printing Scene");
        for (const auto& i : s.models) print(i);
        for (const auto& i : s.modelGroups) print(i);
        for (const auto& i : s.pointLights) print(i);
    }

    template<>
    void print<MaterialFile>(const MaterialFile& m)
    {
        printTitle("Printing Material File");
        for (const auto& t : m.textures) print(t);
        for (const auto& v : m.materials) print(v);
    }

    template<>
    void print<ModelFile>(const ModelFile& m)
    {
        printTitle("Printing Model File");
        print(m.id);
        for (const auto& g : m.geometries) print(g);
        for (const auto& v : m.models) print(v);
    }

    template<>
    void print<SceneFile>(const SceneFile& s)
    {
        printTitle("Printing Scene File");
        print(s.extraMaterials);
        print(s.extraModels);
        print(s.scene);
    }
}
