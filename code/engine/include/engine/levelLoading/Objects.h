#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <box2d/id.h>


struct GameObject
{
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    std::string tag = "undefined";
    bool isTriangle = false;
    std::string textureName;
    glm::vec4 uv = {0, 0, 1, 1};
};

struct LevelMeta
{
    int id = -1.0;
    std::string name;
    std::string fileName;
};

struct Level
{
    float velocityMultiplier = 1.f;
    std::vector<GameObject> objects;
};

