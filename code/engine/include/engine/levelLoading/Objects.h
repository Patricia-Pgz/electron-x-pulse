#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


struct GameObject
{
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    std::string tag = "undefined";
    bool isTriangle = false;
    std::string textureName;
    glm::vec3 scale = {1.f, 1.f, 1.f};
    glm::vec4 uv = {0, 0, 1, 1};
    float rotation = 0.f;
    bool enableCollision = true;
};

struct LevelMeta
{
    int id = -1.0;
    std::string name;
    std::string fileName;
};

struct Level
{
    std::string audioFile;
    float velocityMultiplier = 1.f;
    float currentLevelSpeed = 1.f;
    float groundLevel = 0.f;
    std::vector<GameObject> backgrounds;
    std::vector<GameObject> objects;
};

