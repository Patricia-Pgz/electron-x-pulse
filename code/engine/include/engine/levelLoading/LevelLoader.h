#pragma once
#include <string>
#include <vector>
#include <glaze/glaze.hpp>
#include <box2d/id.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "engine/Assets.h"
#include "engine/rendering/Texture.h"

namespace gl3::engine::levelLoading
{
    struct GameObject
    {
        glm::vec3 position = {0.0f, 0.0f, 0.0f};
        glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        std::string tag = "undefined";
        b2WorldId physicsWorld = b2_nullWorldId;
        bool isTriangle = false;
        rendering::Texture* texture = nullptr;
        glm::vec4 uv = {0, 0, 1, 1};
    };

    struct Level
    {
        std::string name;
        int scrollSpeed;
        int playerX, playerY;
        std::vector<GameObject> objects;
        std::string filename;
    };

    class LevelLoader
    {
    public:
        static void loadAllLevels();
        static const std::vector<Level>& getLevels() { return levels; }

    private:
        static std::vector<Level> levels;

        static std::vector<GameObject> loadGameObjects(const std::string& file)
        {
            std::ifstream f(file);
            std::string json((std::istreambuf_iterator<char>(f)),
                             std::istreambuf_iterator<char>());

            std::vector<GameObject> objs;
            glz::read_json(objs, json);
            return objs;
        }

        static Level loadLevelFromFile(const std::string& filename);
    };
}

template <>
struct glz::meta<gl3::engine::levelLoading::GameObject>
{
    using T = gl3::engine::levelLoading::GameObject;
    static constexpr auto value = glz::object(
        "position", &T::position,
        "color", &T::color,
        "tag", &T::tag,
        "isTriangle", &T::isTriangle,
        "texture", &T::texture,
        "uv", &T::uv
    );
};
