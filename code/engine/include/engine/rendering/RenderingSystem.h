#pragma once
#include <entt/entt.hpp>
#include "engine/ecs/EntityFactory.h"
#include "engine/ecs/System.h"
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
            const glm::mat4 projection = MVPMatrixHelper::calculateProjectionMatrix(context.getWindowBounds());

            return projection * view * model;
        }

        void draw() const
        {
            if (!is_active) { return; }

            auto& registry = game_.getRegistry();
            auto& context = game_.getContext();
            for (const auto& entities = registry.view<ecs::TransformComponent, ecs::RenderComponent>(); const auto&
                 entity : entities)
            {
                auto& transform = entities.get<ecs::TransformComponent>(entity);
                auto& renderComp = entities.get<ecs::RenderComponent>(entity);

                if (context.isInVisibleWindow(transform.position) && renderComp.isActive)
                {
                    const auto mvpMatrix = calculateMvpMatrix(transform.position, transform.zRotation, transform.scale,
                                                              context);
                    renderComp.shader.use();
                    renderComp.shader.setMat4("mvp", mvpMatrix);
                    renderComp.shader.setVector4("color", renderComp.color);
                    if (renderComp.texture)
                    {
                        renderComp.shader.setInt("useTexture", 1);
                        renderComp.texture->bind(0); // Bind texture to slot 0
                        renderComp.shader.setInt("texture1", 0);
                    }
                    renderComp.mesh.draw();
                }
            }
        };
    };
}
