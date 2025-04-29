#include "engine/rendering/Texture.h"
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include "../../../game/src/Assets.h"
#include "../stb/stb_image.h"

namespace gl3::engine::rendering
{
    Texture::Texture(const std::string& path, const int tilesX, const int tilesY)
    {
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        // Set wrapping and filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(resolveAssetPath(path).c_str(), &width, &height, &nrChannels, 0);
        if (!data)
        {
            throw std::runtime_error("Failed to load texture: " + path);
        }

        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::string filename = std::filesystem::path(path).filename().string();
        std::ranges::transform(filename, filename.begin(), ::tolower);
        is_tileset_ = filename.find("tileset") != std::string::npos;
        tile_uvs_ = isTileSet()? generateTileUVs(tilesX,tilesY) : tile_uvs_;
        stbi_image_free(data);
    }

    Texture::~Texture()
    {
        if (ID != 0)
        {
            glDeleteTextures(1, &ID);
        }
    }

    Texture::Texture(Texture&& other) noexcept
        : ID(other.ID), width(other.width), height(other.height)
    {
        other.ID = 0;
        other.width = 0;
        other.height = 0;
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if (this != &other)
        {
            if (ID != 0)
            {
                glDeleteTextures(1, &ID);
            }
            ID = other.ID;
            width = other.width;
            height = other.height;

            other.ID = 0;
            other.width = 0;
            other.height = 0;
        }
        return *this;
    }

    void Texture::bind(GLuint slot) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    glm::vec4 Texture::getTileUV(int tileX, int tileY, int tilesX, int tilesY)
    {
        const float tileWidth = 1.0f / static_cast<float>(tilesX);
        const float tileHeight = 1.0f / static_cast<float>(tilesY);

        const float u0 = tileX * tileWidth;
        const float v0 = tileY * tileHeight;
        const float u1 = u0 + tileWidth;
        const float v1 = v0 + tileHeight;

        return glm::vec4(u0, v0, u1, v1);
    }

    std::vector<glm::vec4> Texture::generateTileUVs(const int tilesX, const int tilesY)
    {
        std::vector<glm::vec4> uvs;
        uvs.reserve(tilesX * tilesY);

        for (int y = 0; y < tilesY; ++y)
        {
            for (int x = 0; x < tilesX; ++x)
            {
                uvs.push_back(getTileUV(x, y, tilesX, tilesY));
            }
        }

        return uvs;
    }
}
