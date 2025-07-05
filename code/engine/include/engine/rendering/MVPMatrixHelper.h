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
            return glm::lookAt(
                context.getCameraPos(),
                context.getCameraCenter(),
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
        }

        static glm::mat4 calculateProjectionMatrix(const std::vector<float>& windowBounds)
        {
            return glm::ortho(windowBounds[0], windowBounds[1], windowBounds[2], windowBounds[3],
                              0.1f,
                              10.f);
        }

        static glm::mat4 calculateModelMatrix(const glm::vec3& position, const float& zRotationInDegrees,
                                              const glm::vec3& scale)
        {
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, position * pixelsPerMeter); // world to pixel
            model = glm::rotate(model, glm::radians(zRotationInDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, scale * pixelsPerMeter); // scale in pixels
            return model;
        }

        static glm::vec2 screenToWorld(const Game& game, const float screenPosX, const float screenPosY)
        {
            const auto screenSize = ImGui::GetIO().DisplaySize;

            const float ndcX = (screenPosX / screenSize.x) * 2.0f - 1.0f;
            const float ndcY = (screenPosY / screenSize.y) * 2.0f - 1.0f;

            const glm::vec4 clipPos = glm::vec4(ndcX, ndcY, 0.f, 1.f);

            const glm::mat4 projection = calculateProjectionMatrix(game.getContext().getWindowBounds());
            const glm::mat4 view = calculateViewMatrix(game.getContext());

            const glm::mat4 inv = glm::inverse(projection * view);
            const glm::vec4 world = inv * clipPos;

            return {world.x / pixelsPerMeter, world.y / pixelsPerMeter};
        }

        static glm::vec2 toScreen(const Game& game, const float x, const float y)
        {
            // Convert to world-space with scaling
            const glm::vec4 worldPos(x * pixelsPerMeter, y * pixelsPerMeter, 0.f, 1.f);

            const glm::mat4 projection = calculateProjectionMatrix(game.getContext().getWindowBounds());
            const glm::mat4 view = calculateViewMatrix(game.getContext());

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
