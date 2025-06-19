#include "engine/userInterface/FontManager.h"
#include <filesystem>

namespace gl3::engine::ui{
    std::unordered_map<std::string, ImFont*> FontManager::fonts_;

    void FontManager::loadFonts(const std::string& folder) {
        ImGuiIO& io = ImGui::GetIO();
        fonts_["default"] = io.Fonts->AddFontDefault();

        for (const auto& entry : std::filesystem::directory_iterator(folder)) {
            if (entry.is_regular_file()) {
                std::string name = entry.path().stem().string();
                fonts_[name] = io.Fonts->AddFontFromFileTTF(entry.path().string().c_str(), 22);
            }
        }

        //Add custom sized fonts:
        fonts_["pixeloid-bold-26"] = io.Fonts->AddFontFromFileTTF((folder + "/PixeloidSans-Bold.ttf").c_str(), 26);
        fonts_["pixeloid-bold-30"] = io.Fonts->AddFontFromFileTTF((folder + "/PixeloidSans-Bold.ttf").c_str(), 30);
    }

    ImFont* FontManager::getFont(const std::string& name) {
        auto font = fonts_.find(name);
        if (font != fonts_.end())
            return font->second;
                return nullptr;
    }
}

