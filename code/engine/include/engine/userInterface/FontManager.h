#pragma once
#include <unordered_map>
#include <string>
#include <imgui.h>

namespace gl3::engine::ui{

    class FontManager {
    public:
        static void loadFonts(const std::string& folder);
        static ImFont* getFont(const std::string& name);

    private:
        static std::unordered_map<std::string, ImFont*> fonts_;
    };

}
