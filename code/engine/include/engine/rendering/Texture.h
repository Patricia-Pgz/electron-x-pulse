#pragma once

#include <string>
#include <glad/glad.h>


namespace gl3::engine::rendering {

    class Texture {
    public:
        [[nodiscard]] GLuint getId() const { return ID; }
        explicit Texture(const std::string& path);
        void bind(GLuint slot) const;

    private:
        GLuint ID;
    };

}
