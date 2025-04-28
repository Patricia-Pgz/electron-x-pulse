#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace gl3::engine::rendering
{
    class Texture
    {
    public:
        explicit Texture(const std::string& path);
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        void bind(GLuint slot = 0) const;

        [[nodiscard]] GLuint getID() const { return ID; }
        [[nodiscard]] int getWidth() const { return width; }
        [[nodiscard]] int getHeight() const { return height; }

        // For tile-based UVs
        [[nodiscard]] glm::vec4 getTileUV(int tileX, int tileY, int tilesX, int tilesY) const;
        [[nodiscard]] std::vector<glm::vec4> generateTileUVs(int tilesX, int tilesY) const;

    private:
        GLuint ID = 0;
        int width = 0;
        int height = 0;
    };
}
