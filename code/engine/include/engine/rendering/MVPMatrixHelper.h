#pragma once
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/Constants.h"
#include "engine/Context.h"
#include "engine/Game.h"

using gl3::engine::Game;

namespace gl3::engine::rendering
{
    /**
     * @class MVPMatrixHelper
     * @brief Utility class for calculating Model, View, and Projection matrices.
     *
     * Provides static helper methods to build transform matrices for rendering,
     * convert between screen and world coordinates, and handle orthographic projection setup.
     */
    class MVPMatrixHelper
    {
    public:
        /**
         * @brief Computes the view matrix based on the camera position.
         * @param context The rendering context providing camera information.
         * @return The view matrix translating the scene by the inverse camera position.
         */
        static glm::mat4 calculateViewMatrix(const context::Context& context)
        {
            return translate(glm::mat4(1.0f), -context.getCameraPos());
        }

        /**
         * @brief Computes the orthographic projection matrix for the current window size.
         * @param context The rendering context providing camera and window information.
         * @return The orthographic projection matrix in world space.
         */
        static glm::mat4 calculateProjectionMatrix(const context::Context& context)
        {
            int width;
            int height;
            glfwGetWindowSize(context.getWindow(), &width, &height);
            const auto fWidth = static_cast<float>(width);
            const auto fHeight = static_cast<float>(height);
            const glm::vec2 camPos = context.getCameraPos();

            return glm::ortho(camPos.x - (fWidth * 0.5f), camPos.x + (fWidth * 0.5f),
                              camPos.y - fHeight * 0.5f,
                              camPos.y + fHeight * 0.5f,
                              0.1f,
                              10.f);
        }

        /**
         * @brief Builds a model matrix for an object given its position, rotation, and scale.
         *
         * Applies translation, rotation (around Z-axis), and scaling.
         *
         * @param position Position in world space (meters).
         * @param zRotationInDegrees Rotation around Z axis in degrees.
         * @param scale Object scale.
         * @return The combined model matrix.
         */
        static glm::mat4 calculateModelMatrix(const glm::vec3& position, const float& zRotationInDegrees,
                                              const glm::vec3& scale)
        {
            auto model = glm::mat4(1.0f);

            model = translate(model, {position.x * pixelsPerMeter, position.y * pixelsPerMeter, position.z});
            model = rotate(model, glm::radians(zRotationInDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, scale * pixelsPerMeter);
            return model;
        }

        /**
         * @brief Calculate the full MVP matrix for an entity.
         *
         * Combines Model, View, and Projection transforms.
         *
         * @param position World position in meters.
         * @param zRotationInDegrees Rotation around Z axis in degrees.
         * @param scale Scale vector.
         * @param context The rendering context (camera, window).
         * @return Combined MVP matrix.
         */
        static glm::mat4 calculateMvpMatrix(const glm::vec3& position, const float& zRotationInDegrees,
                                            const glm::vec3& scale, const context::Context& context)
        {
            const auto model = calculateModelMatrix(position, zRotationInDegrees, scale);
            const glm::mat4 view = calculateViewMatrix(context);
            const glm::mat4 projection = calculateProjectionMatrix(context);

            return projection * view * model;
        }

        /**
         * @brief Converts a screen-space position (e.g. mouse) to world-space coordinates.
         *
         * @param context The rendering context.
         * @param screenPosX X coordinate in screen space (pixels).
         * @param screenPosY Y coordinate in screen space (pixels).
         * @return The corresponding world-space position.
         */
        static glm::vec2 screenToWorld(const context::Context& context, const float screenPosX, const float screenPosY)
        {
            int width, height;
            glfwGetWindowSize(context.getWindow(), &width, &height);

            const float ndcX = (screenPosX / static_cast<float>(width)) * 2.0f - 1.0f;
            const float ndcY = 1.0f - (screenPosY / static_cast<float>(height)) * 2.0f;

            const glm::vec4 clipPos = glm::vec4(ndcX, ndcY, 0.f, 1.f);

            const glm::mat4 projection = calculateProjectionMatrix(context);
            const glm::mat4 view = calculateViewMatrix(context);

            const glm::mat4 inv = glm::inverse(projection * view);
            const glm::vec4 world = inv * clipPos;

            return {world.x / pixelsPerMeter, world.y / pixelsPerMeter};
        }

        /**
         * @brief Converts a world-space position to screen-space coordinates.
         *
         * @param context The rendering context.
         * @param x X coordinate in world space.
         * @param y Y coordinate in world space.
         * @return The corresponding position in screen space (pixels).
         */
        static glm::vec2 toScreen(const context::Context& context, const float x, const float y)
        {
            const glm::vec4 worldPos(x * pixelsPerMeter, y * pixelsPerMeter, 0.f, 1.f);

            const glm::mat4 projection = calculateProjectionMatrix(context);
            const glm::mat4 view = calculateViewMatrix(context);

            const glm::vec4 clip = projection * view * worldPos;

            const ImVec2 screenSize = ImGui::GetIO().DisplaySize;
            const float ndcX = clip.x / clip.w;
            const float ndcY = clip.y / clip.w;

            float screenX = (ndcX * 0.5f + 0.5f) * screenSize.x;
            float screenY = (1.0f - (ndcY * 0.5f + 0.5f)) * screenSize.y;

            return {screenX, screenY};
        }
    };
}
