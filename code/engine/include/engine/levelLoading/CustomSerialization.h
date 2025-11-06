/**
* @file CustomSerialization.h
 * @brief Specializations of glz::meta for serializing and deserializing custom types using the Glaze library.
 *
 * This file provides meta definitions for glm vectors and game-specific data structures such as
 * GameObject, GameObjectGroup, LevelMeta, and Level to enable JSON (or other format) serialization
 * and deserialization with glaze.
 */
#pragma once
#include <glaze/glaze.hpp>
#include <glm/glm.hpp>
#include "Objects.h"


/// Specialization of glz::meta for glm::vec3, maps x, y, z as an array.
template <>
struct glz::meta<glm::vec3>
{
    using T = glm::vec3;
    static constexpr auto value = glz::array(&T::x, &T::y, &T::z);
};

/// Specialization of glz::meta for glm::vec4, maps x, y, z, w as an array.
template <>
struct glz::meta<glm::vec4>
{
    using T = glm::vec4;
    static constexpr auto value = glz::array(&T::x, &T::y, &T::z, &T::w);
};

/// Specialization of glz::meta for GameObjectGroup, serializes as an object with name, children, and colliderAABB fields.
template <>
struct glz::meta<GameObjectGroup>
{
    using T = GameObjectGroup;
    static constexpr auto value = glz::object(
        "ID", &T::ID,
        "children", &T::children,
        "parent", &T::parent
    );
};

/// Specialization of glz::meta for GameObject, serializes multiple fields including transform, appearance, and behavior flags.
template <>
struct glz::meta<GameObject>
{
    using T = GameObject;
    static constexpr auto value = glz::object(
        "position", &T::position,
        "scale", &T::scale,
        "rotation", &T::zRotation,
        "color", &T::color,
        "tag", &T::tag,
        "isTriangle", &T::isTriangle,
        "textureName", &T::textureName,
        "uv", &T::uv,
        "generatePhysicsComp", &T::generatePhysicsComp,
        "generateRenderComp", &T::generateRenderComp,
        "vertexShaderPath", &T::vertexShaderPath,
        "fragmentShaderPath", &T::fragmentShaderPath,
        "gradientTopColor", &T::gradientTopColor,
        "gradientBottomColor", &T::gradientBottomColor,
        "parallaxFactor", &T::parallaxFactor,
        "zLayer", &T::zLayer,
        "isSensor", &T::isSensor,
        "repeatTextureX", &T::repeatTextureX
    );
};

/// Specialization of glz::meta for LevelMeta, serializes metadata about a level.
template <>
struct glz::meta<LevelMeta>
{
    using T = LevelMeta;
    static constexpr auto value = object(
        "id", &T::id,
        "name", &T::name,
        "levelFile", &T::fileName,
        "previewImg", &T::previewImageName
    );
};

/// Specialization of glz::meta for Level, serializes a complete level including audio, environment, and objects.
template <>
struct glz::meta<Level>
{
    using T = Level;
    static constexpr auto value = glz::object(
        "audioFile", &T::audioFileName,
        "velocityMultiplier", &T::velocityMultiplier,
        "playerStartPosX", &T::playerStartPosX,
        "groundLevel", &T::groundLevel,
        "clearColor", &T::clearColor,
        "gradientTopColor", &T::gradientTopColor,
        "gradientBottomColor", &T::gradientBottomColor,
        "backgrounds", &T::backgrounds,
        "groups", &T::groups,
        "objects", &T::objects,
        "currentGroupIDs", &T::currentGroupIDs
    );
};
