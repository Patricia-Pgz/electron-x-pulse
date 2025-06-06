#pragma once

#include <string>
#include <vector>

#include "box2d/id.h"
#include "engine/Assets.h"
#include "engine/rendering/Texture.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace gl3::engine::levelLoading
{
    struct GameObject
    {
        const glm::vec3& position = {0.0f, 0.0f, 0.0f};
        const glm::vec4& color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        const std::string& tag = "undefined";
        const b2WorldId& physicsWorld = b2_nullWorldId;
        const bool isTriangle = false;
        const rendering::Texture* texture = nullptr;
        const glm::vec4& uv = {0, 0, 1, 1};
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

        static Level loadLevelFromFile(const std::string& filename);
    };
}
