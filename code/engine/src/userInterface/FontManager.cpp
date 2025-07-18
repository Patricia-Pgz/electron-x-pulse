/**
* @file FontManager.cpp
 * @brief Implements the FontManager for loading and retrieving ImGui fonts.
 */
#include "engine/userInterface/FontManager.h"
#include <filesystem>

namespace gl3::engine::ui
{
    /// Static map storing loaded fonts by name.
    std::unordered_map<std::string, ImFont*> FontManager::fonts;

    void FontManager::loadFonts(const std::string& folder)
    {
        const ImGuiIO& io = ImGui::GetIO();
        // Add ImGui's built-in default font.
        fonts["default"] = io.Fonts->AddFontDefault();

        // Add all TTF files from the folder.
        for (const auto& entry : std::filesystem::directory_iterator(folder))
        {
            if (entry.is_regular_file())
            {
                std::string name = entry.path().stem().string();
                fonts[name] = io.Fonts->AddFontFromFileTTF(entry.path().string().c_str(), 22);
            }
        }

        //Add custom sized fonts:
        fonts["pixeloid-bold-26"] = io.Fonts->AddFontFromFileTTF((folder + "/PixeloidSans-Bold.ttf").c_str(), 26);
        fonts["pixeloid-bold-30"] = io.Fonts->AddFontFromFileTTF((folder + "/PixeloidSans-Bold.ttf").c_str(), 30);
    }

    ImFont* FontManager::getFont(const std::string& name)
    {
        if (const auto font = fonts.find(name); font != fonts.end())
            return font->second;
        return nullptr;
    }
}

