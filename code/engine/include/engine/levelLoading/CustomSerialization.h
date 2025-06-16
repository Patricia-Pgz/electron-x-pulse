#pragma once
#include <glaze/glaze.hpp>
#include <glm/glm.hpp>
#include "Objects.h"


template <>
struct glz::meta<glm::vec3>
{
    using T = glm::vec3;
    static constexpr auto value = glz::array(&T::x, &T::y, &T::z);
};

template <>
struct glz::meta<glm::vec4>
{
    using T = glm::vec4;
    static constexpr auto value = glz::array(&T::x, &T::y, &T::z, &T::w);
};

template <>
struct glz::meta<GameObject>
{
    using T = GameObject;
    static constexpr auto value = glz::object(
        "position", &T::position,
        "scale", &T::scale,
        "color", &T::color,
        "tag", &T::tag,
        "isTriangle", &T::isTriangle,
        "textureName", &T::textureName,
        "uv", &T::uv,
        "enableCol", &T::enableCollision
    );
};

template <>
struct glz::meta<LevelMeta>
{
    using T = LevelMeta;
    static constexpr auto value = object(
        "id", &T::id,
        "name", &T::name,
        "levelFile", &T::fileName
    );
};

template <>
struct glz::meta<Level>
{
    using T = Level;
    static constexpr auto value = glz::object(
        "audioFile", &T::audioFile,
        "velocityMultiplier", &T::velocityMultiplier,
        "groundLevel", &T::groundLevel,
        "backgrounds", &T::backgrounds,
        "objects", &T::objects
    );
};
