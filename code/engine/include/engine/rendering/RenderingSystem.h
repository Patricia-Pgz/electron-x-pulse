#pragma once
#include <entt/entt.hpp>
#include "engine/ecs/EntityFactory.h"
#include "engine/ecs/System.h"
#include "engine/levelloading/LevelManager.h"
#include "engine/rendering/MVPMatrixHelper.h"
#include "glm/gtc/epsilon.hpp"

namespace gl3::engine::rendering
{
    /**
     * @class RenderingSystem
     * @brief ECS system responsible for rendering all entities with a RenderComponent.
     *
     * Uses an MVP matrix to transform objects and handles texture binding,
     * shader uniform setup, parallax scrolling, and gradient skies.
     */
    class RenderingSystem final : public ecs::System
    {
    public:
        /**
         * @brief Construct a new RenderingSystem.
         * @param game Reference to the main game instance.
         */
        explicit RenderingSystem(Game& game) : System(game)
        {
        };

        /**
         * @brief Render all visible entities with active RenderComponents.
         *
         * Draws each entity using its mesh, texture, shader, and transform.
         * Applies parallax UV offset if enabled. Skips rendering if the entity
         * is outside the visible window.
         */
        void draw() const
        {
            if (!is_active) { return; }

            auto& registry = game.getRegistry();
            const auto& context = game.getContext();

            for (const auto& entities = registry.view<
                     ecs::TransformComponent, ecs::RenderComponent, ecs::TagComponent>(); const auto&
                 entity : entities)
            {
                auto& transform = entities.get<ecs::TransformComponent>(entity);
                auto& renderComp = entities.get<ecs::RenderComponent>(entity);
                auto tag = entities.get<ecs::TagComponent>(entity).tag;

                // Render object if in view
                if (context.isInVisibleWindow(transform.position, transform.scale) && renderComp.isActive)
                {
                    const auto mvpMatrix = MVPMatrixHelper::calculateMvpMatrix(transform.position, transform.zRotation, transform.scale,
                                                              context);
                    renderComp.shader.use();
                    renderComp.shader.setMat4("mvp", mvpMatrix);
                    renderComp.shader.setVector4("color", renderComp.color);

                    // If gradient top and bottom are not the same color -> Handle color gradient
                    if (!all(epsilonEqual(renderComp.gradientTopColor, renderComp.gradientBottomColor, 0.001f)))
                    {
                        renderComp.shader.setVector4("topColor", renderComp.gradientTopColor);
                        renderComp.shader.setVector4("bottomColor", renderComp.gradientBottomColor);
                    }

                    // Bind and setup texture if available
                    if (renderComp.texture)
                    {
                        renderComp.shader.setInt("useTexture", 1);
                        renderComp.texture->bind(0);
                        renderComp.shader.setInt("texture1", 0);

                        // Handle parallax UV offset if enabled and game is running
                        if (transform.parallaxFactor != 0 && !game.isPaused())
                        {
                            const float pixelsPerSecond = levelLoading::LevelManager::getMostRecentLoadedLevel()->
                                currentLevelSpeed * pixelsPerMeter;

                            const float uvPerSecond = pixelsPerSecond / static_cast<float>(renderComp.texture->
                                                                                                      getWidth());

                            renderComp.uvOffset.x += transform.parallaxFactor * uvPerSecond * game.getDeltaTime();
                            renderComp.uvOffset.x = std::fmod(renderComp.uvOffset.x, 1.0f);
                            if (renderComp.uvOffset.x < 0) renderComp.uvOffset.x += 1.0f;

                            renderComp.shader.setVec2("uvOffset", renderComp.uvOffset);
                        }
                    }

                    renderComp.mesh.draw();
                }
            }
        };
    };
} // namespace gl3::engine::rendering
