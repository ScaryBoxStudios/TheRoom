#include "LoadingScreen.hpp"
WARN_GUARD_ON
#include "../Graphics/Image/ImageLoader.hpp"
#include "../Graphics/Geometry/ModelLoader.hpp"
WARN_GUARD_OFF
#include "../Util/FileLoad.hpp"

// BufferType for the files loaded
using BufferType = std::vector<std::uint8_t>;

void LoadingScreen::onInit(ScreenContext& sc)
{
    // Store the engine reference
    mEngine = sc.GetEngine();

    // Load the textures
    LoadTextures();

    // Load the models
    LoadModels();

    // Load font
    mEngine->GetRenderer().GetTextRenderer().GetFontStore().LoadFont("visitor", "ext/visitor.ttf");

    // Load the skybox
    auto skybox = std::make_unique<Skybox>();
    ImageLoader imLoader;
    skybox->Load(
        {
            { Skybox::Target::Right,  imLoader.LoadFile("ext/Skybox/right.jpg")  },
            { Skybox::Target::Left,   imLoader.LoadFile("ext/Skybox/left.jpg")   },
            { Skybox::Target::Top,    imLoader.LoadFile("ext/Skybox/top.jpg")    },
            { Skybox::Target::Bottom, imLoader.LoadFile("ext/Skybox/bottom.jpg") },
            { Skybox::Target::Back,   imLoader.LoadFile("ext/Skybox/back.jpg")   },
            { Skybox::Target::Front,  imLoader.LoadFile("ext/Skybox/front.jpg")  }
        }
    );
    sc.GetEngine()->SetSkybox(std::move(skybox));

    // Call finish
    mFinishCb();
}

void LoadingScreen::LoadTextures()
{
    // Retrieve the textureStore from the renderer
    auto& textureStore = mEngine->GetRenderer().GetTextureStore();

    // The ImageLoader object
    ImageLoader imageLoader;

    // The texture map
    std::unordered_map<std::string, std::string> textures =
    {
        {"ext/mahogany_wood.jpg",            "mahogany_wood"},
        {"ext/mahogany_wood_spec.jpg",       "mahogany_wood_spec"},
        {"ext/WoodenCabin/WoodCabinDif.jpg", "house_diff"},
        {"ext/WoodenCabin/WoodCabinSM.jpg",  "house_spec"},
        {"ext/Dungeon/maps/Wall1_T.tga",     "well_diff"},
        {"ext/Dungeon/maps/Wall1_B.tga",     "well_spec"}
    };

    // Load the textures
    for (const auto& p : textures)
    {
        RawImage<> pb = imageLoader.LoadFile(p.first);
        textureStore.Load(p.second, pb);
    }
}

void LoadingScreen::LoadModels()
{
    // Retrieve the model and texture stores from the renderer
    auto& renderer = mEngine->GetRenderer();
    auto& modelStore = renderer.GetModelStore();
    auto& textureStore = renderer.GetTextureStore();

    // Model loader instance
    ModelLoader modelLoader;

    struct MdlData
    {
        std::string filepath;
        std::string extension;
        std::string name;
        Material& material;
    };

    // Create materials
    Material mahogany;
    mahogany.SetDiffuseTexture(textureStore["mahogany_wood"]->texId);
    mahogany.SetSpecularTexture(textureStore["mahogany_wood_spec"]->texId);

    Material white;
    white.SetDiffuseColor(glm::vec3(0xFF));

    Material house;
    house.SetDiffuseTexture(textureStore["house_diff"]->texId);
    house.SetSpecularTexture(textureStore["house_spec"]->texId);

    Material well;
    well.SetDiffuseTexture(textureStore["well_diff"]->texId);
    well.SetSpecularTexture(textureStore["well_spec"]->texId);

    std::vector<MdlData> models = {
        { "ext/Cube/cube.obj",               "obj", "cube",   mahogany } // Cube
    ,   { "ext/teapot.obj",                  "obj", "teapot", white    } // Teapot
    ,   { "ext/WoodenCabin/WoodenCabin.dae", "dae", "house",  house    } // House
    ,   { "ext/Dungeon/Well.obj",            "obj", "well",   well     } // Well
    };

    for(auto& m : models)
    {
        auto file = FileLoad<BufferType>(m.filepath);
        if(!file)
            throw std::runtime_error("Couldn't load file (" + m.filepath + ")");

        ModelData model = modelLoader.Load(*file, m.extension.c_str());
        if(model.meshes.size() == 0)
            throw std::runtime_error("Couldn't load model (" + m.filepath + ")");

        modelStore.Load(m.name, std::move(model));
        modelStore[m.name]->material = m.material;
    }
}

void LoadingScreen::onUpdate(float dt)
{
    (void) dt;
}

void LoadingScreen::onRender(float interpolation)
{
    (void) interpolation;
}

void LoadingScreen::onShutdown()
{
}

void LoadingScreen::SetFinishCb(FinishCb cb)
{
    mFinishCb = cb;
}
