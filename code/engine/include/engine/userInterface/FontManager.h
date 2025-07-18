#pragma once
#include <unordered_map>
#include <string>
#include <imgui.h>

namespace gl3::engine::ui {

    /**
     * @class FontManager
     * @brief Manages ImGui fonts for the application.
     *
     * Provides functionality to load fonts from assets/fonts and retrieve loaded fonts by name.
     * Fonts are stored in a static cache for reuse.
     */
    class FontManager {
    public:
        /**
         * @brief Loads all fonts from the specified folder. (e.g. assets/fonts)
         *
         * This function searches the given folder for supported font files
         * and loads them into the internal cache.
         *
         * @param folder Path to the folder containing font files.
         */
        static void loadFonts(const std::string& folder);

        /**
         * @brief Retrieves a loaded font by name.
         *
         * @param name Name of the font to retrieve.
         * @return Pointer to the ImFont object, or nullptr if not found.
         */
        static ImFont* getFont(const std::string& name);

    private:
        /**
         * @brief Internal font cache mapping font names to ImGui font pointers.
         */
        static std::unordered_map<std::string, ImFont*> fonts_;
    };

}
