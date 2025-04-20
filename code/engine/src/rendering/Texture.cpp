#include "engine/rendering/Texture.h"
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include "../../../game/src/Assets.h"
#include "../stb/stb_image.h"

namespace gl3::engine::rendering
{
    Texture::Texture(const std::string& path) {
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        // Set filtering/wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(resolveAssetPath(path).c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            throw std::runtime_error("Failed to load texture: " + path);
        }
        stbi_image_free(data);
    }

    void Texture::bind(GLuint slot = 0) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, ID);
    }
}
