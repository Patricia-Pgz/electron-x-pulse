#pragma once
#include <entt/entt.hpp>
#include "engine/ecs/EntityFactory.h"
#include "engine/ecs/System.h"
#include "engine/levelloading/LevelManager.h"
#include "engine/rendering/MVPMatrixHelper.h"

namespace gl3::engine::rendering
{
    class RenderingSystem final : public ecs::System
    {
    public:
        explicit RenderingSystem(Game& game) : System(game)
        {
        };

        static glm::mat4 calculateMvpMatrix(const glm::vec3& position, const float& zRotationInDegrees,
                                            const glm::vec3& scale, const context::Context& context)
        {
            const auto model = MVPMatrixHelper::calculateModelMatrix(position, zRotationInDegrees, scale);
            const glm::mat4 view = MVPMatrixHelper::calculateViewMatrix(context);
            const glm::mat4 projection = MVPMatrixHelper::calculateProjectionMatrix(context);

            return projection * view * model;
        }

        /**
         *Render entities from the registry with active render component, until they leave the screen to the left
         */
        void draw() const
        {
            if (!is_active) { return; }

            auto& registry = game_.getRegistry();
            auto& context = game_.getContext();
            for (const auto& entities = registry.view<
                     ecs::TransformComponent, ecs::RenderComponent, ecs::TagComponent>(); const auto&
                 entity : entities)
            {
                auto& transform = entities.get<ecs::TransformComponent>(entity);
                auto& renderComp = entities.get<ecs::RenderComponent>(entity);
                auto tag = entities.get<ecs::TagComponent>(entity).tag;

                //render object if in view
                if (context.isInVisibleWindow(transform.position, transform.scale) && renderComp.isActive)
                {
                    const auto mvpMatrix = calculateMvpMatrix(transform.position, transform.zRotation, transform.scale,
                                                              context);
                    renderComp.shader.use();
                    renderComp.shader.setMat4("mvp", mvpMatrix);
                    renderComp.shader.setVector4("color", renderComp.color);
                    //Make a sky gradient (with gradient shaders)
                    if (tag == "sky")
                    {
                        renderComp.shader.setVector4("topColor", renderComp.gradientTopColor);
                        renderComp.shader.setVector4("bottomColor", renderComp.gradientBottomColor);
                    }
                    if (renderComp.texture)
                    {
                        renderComp.shader.setInt("useTexture", 1);
                        renderComp.texture->bind(0); // Bind texture to slot 0
                        renderComp.shader.setInt("texture1", 0);
                        if (transform.parallaxFactor != 0 && !game_.isPaused())
                        {
                            float pixelsPerSecond = levelLoading::LevelManager::getMostRecentLoadedLevel()->
                                currentLevelSpeed * pixelsPerMeter;
                            float uvPerSecond = pixelsPerSecond / static_cast<float>(renderComp.texture->
                                getWidth());
                            renderComp.uvOffset.x += transform.parallaxFactor * uvPerSecond * game_.getDeltaTime();
                            renderComp.uvOffset.x = std::fmod(renderComp.uvOffset.x, 1.0f);
                            renderComp.shader.setVec2("uvOffset", renderComp.uvOffset);
                        }
                    }
                    renderComp.mesh.draw();
                }
            }
        };
    };
}
