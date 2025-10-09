/**
* @file Objects.h
 * @brief Defines core data structures used for representing and loading level data in the game.
 *
 * This file contains definitions for GameObject, GameObjectGroup, LevelMeta, and Level structs,
 * which encapsulate the properties of game objects, their grouping, metadata, and the overall
 * level configuration including audio, gameplay parameters, and visual elements.
 *
 * These structures are primarily used for level loading and saving, serialization, and editor manipulation
 * of level content.
 */
#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

/**
 * @brief Represents a single game object with transform, rendering, and physics properties.
 */
struct GameObject
{
    glm::vec3 position = {0.0f, 0.0f, 0.0f}; /**< World position of the object */
    glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); /**< Base color of the object, if no texture */
    std::string tag = "undefined"; /**< Tag string used for identification */
    bool isTriangle = false; /**< Whether the object is rendered as a triangle or quad*/
    std::string textureName; /**< Texture resource file name @note Needs to be in assets/textures! */
    glm::vec3 scale = {1.f, 1.f, 0.1f}; /**< Scale in x, y, z directions */
    glm::vec4 uv = {0.0f, 0.f, 1.f, 1.f}; /**< UV coordinates for texturing */
    float zRotation = 0.f; /**< Z Rotation in degrees */
    bool generatePhysicsComp = true; /**< Flag to generate physics component */
    bool generateRenderComp = true; /**< Flag to generate render component */
    float parallaxFactor = 0.f; /**< Parallax scrolling factor */
    std::string vertexShaderPath; /**< Path to vertex shader */
    std::string fragmentShaderPath; /**< Path to fragment shader */
    glm::vec4 gradientTopColor = {1.f, 1.f, 1.f, 1.f}; /**< Top gradient color @note no engine implementation yet, what to do with this*/
    glm::vec4 gradientBottomColor = {1.f, 1.f, 1.f, 1.f}; /**< Bottom gradient color @note no engine implementation yet, what to do with this*/
    float zLayer = 0.f;
    bool isSensor = false;/**< Flag to generate physics component as sensor*/
    bool repeatTextureX = false; /**< Flag to repeat the texture on x-axis*/
};

/**
 * @brief Grouping of game objects with a name and an axis-aligned bounding box collider.
 */
struct GameObjectGroup
{
    std::string name; /**< Name identifier for the group */
    std::vector<GameObject> children; /**< Child objects within the group */
    GameObject colliderAABB; /**< Axis-aligned bounding box collider for the group */
};

/**
 * @brief Metadata describing a level.
 */
struct LevelMeta
{
    int id = -1; /**< Unique level identifier */
    std::string name; /**< Level name */
    std::string fileName; /**< Filename for the level data @note File needs to be in assets/levels*/
    std::string previewImageName; /**< Filename for the preview image @note File needs to be in assets/uiTextures*/
};

/**
 * @brief Complete level data including audio, gameplay parameters, backgrounds, groups, and objects.
 */
struct Level
{
    std::string audioFileName; /**< Background audio file name @note file needs to be in assets/audio */
    float velocityMultiplier = 1.f; /**< Speed multiplier for level progression */
    float playerStartPosX = 0.f; /**< Player's starting X position */
    float groundLevel = 0.f; /**< Y border of the ground */
    glm::vec4 clearColor = {1.f, 1.f, 1.f, 1.f}; /**< Background clear color */
    glm::vec4 gradientTopColor = {1.f, 1.f, 1.f, 1.f}; /**< Top color for background gradient for custom implementation in game*/
    glm::vec4 gradientBottomColor = {1.f, 1.f, 1.f, 1.f}; /**< Bottom color for background gradient for custom implementation in game*/
    std::vector<GameObject> backgrounds; /**< Background objects */
    std::vector<GameObjectGroup> groups; /**< Groups of objects */
    std::vector<GameObject> objects; /**< Individual game objects */
    float currentLevelSpeed = 1.f; /**< Current speed of the level */
    float levelLength = 0.f; /**< Length of the level in seconds*/
    float finalBeatIndex = 0.f; /**< Final beat index for synchronization */
};
