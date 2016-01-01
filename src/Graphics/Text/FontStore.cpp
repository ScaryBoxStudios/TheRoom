#include "FontStore.hpp"
#include "../../Util/FileLoad.hpp"

void FontStore::LoadFont(const std::string& name, const std::string& filename)
{
    auto file = FileLoad<Font::BufferType>(filename);
    if (!file)
        throw std::runtime_error("Could not find file: \n" + filename);

    Font font;
    font.Load(*file);

    mFonts.emplace(name, std::move(font));
}

const Font* FontStore::operator[](const std::string& name)
{
    const auto& it = mFonts.find(name);
    if (it != mFonts.end())
        return &it->second;
    else
        return nullptr;
}
