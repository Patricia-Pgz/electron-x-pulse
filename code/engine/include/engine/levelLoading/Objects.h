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
    bool generatePhysicsComp = true;
    bool generateRenderComp = true;
    float parallaxFactor = 0.f;
    std::string vertexShaderPath;
    std::string fragmentShaderPath;
    glm::vec4 gradientTopColor = {1, 1, 1, 1};
    glm::vec4 gradientBottomColor = {1, 1, 1, 1};
};

struct GameObjectGroup
{
    std::string name;
    std::vector<GameObject> children;
    GameObject colliderAABB;
};

struct LevelMeta
{
    int id = -1.0;
    std::string name;
    std::string fileName;
    std::string previewImageName;
};

struct Level
{
    std::string audioFile;
    float velocityMultiplier = 1.f;
    float playerStartPosX = 0.f;
    float groundLevel = 0.f;
    glm::vec4 clearColor = {1, 1, 1, 1};
    glm::vec4 gradientTopColor = {1, 1, 1, 1};
    glm::vec4 gradientBottomColor = {1, 1, 1, 1};
    std::vector<GameObject> backgrounds;
    std::vector<GameObjectGroup> groups;
    std::vector<GameObject> objects;
    float currentLevelSpeed = 1.f;
    float levelLength = 0.f;
    float finalBeatIndex = 0.f;
};

