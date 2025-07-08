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
    class MVPMatrixHelper
    {
    public:
        static glm::mat4 calculateViewMatrix(const context::Context& context)
        {
            return glm::translate(glm::mat4(1.0f), -context.getCameraPos());
        }

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

        static glm::mat4 calculateModelMatrix(const glm::vec3& position, const float& zRotationInDegrees,
                                              const glm::vec3& scale)
        {
            auto model = glm::mat4(1.0f);
            // world to pixel, z only for layering

            model = glm::translate(model, {position.x * pixelsPerMeter, position.y * pixelsPerMeter, position.z});
            model = glm::rotate(model, glm::radians(zRotationInDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, scale * pixelsPerMeter); // scale in pixels
            return model;
        }

        static glm::vec2 screenToWorld(const context::Context& context, const float screenPosX, const float screenPosY)
        {
            const auto screenSize = ImGui::GetIO().DisplaySize;

            const float ndcX = (screenPosX / screenSize.x) * 2.0f - 1.0f;
            const float ndcY = (screenPosY / screenSize.y) * 2.0f - 1.0f;

            const glm::vec4 clipPos = glm::vec4(ndcX, ndcY, 0.f, 1.f);

            const glm::mat4 projection = calculateProjectionMatrix(context);
            const glm::mat4 view = calculateViewMatrix(context);

            const glm::mat4 inv = glm::inverse(projection * view);
            const glm::vec4 world = inv * clipPos;

            return {world.x / pixelsPerMeter, world.y / pixelsPerMeter};
        }

        static glm::vec2 toScreen(const context::Context& context, const float x, const float y)
        {
            // Convert to world-space with scaling
            const glm::vec4 worldPos(x * pixelsPerMeter, y * pixelsPerMeter, 0.f, 1.f);

            const glm::mat4 projection = calculateProjectionMatrix(context);
            const glm::mat4 view = calculateViewMatrix(context);

            // Transform to clip space
            const glm::vec4 clip = projection * view * worldPos;

            // Normalize screen coordinates
            const ImVec2 screenSize = ImGui::GetIO().DisplaySize;
            const float ndcX = clip.x / clip.w;
            const float ndcY = clip.y / clip.w;

            float screenX = (ndcX * 0.5f + 0.5f) * screenSize.x;
            float screenY = (ndcY * 0.5f + 0.5f) * screenSize.y;

            return {screenX, screenY};
        }
    };
}
