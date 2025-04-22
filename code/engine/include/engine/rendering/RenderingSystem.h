#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/ecs/EntityFactory.h"
#include "engine/ecs/System.h"

namespace gl3::engine::rendering
{
    class RenderingSystem : public ecs::System
    {
    public:
        explicit RenderingSystem(Game& game) : System(game){};

        static glm::mat4 calculateMvpMatrix(const glm::vec3& position, const float& zRotationInDegrees,
                                            const glm::vec3& scale, const context::Context& context)
        {
            auto model = glm::mat4(1.0f);
            model = translate(model, position);
            model = rotate(model, glm::radians(zRotationInDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, scale);

            const glm::mat4 view = lookAt(context.getCameraPos(),
                                          context.getCameraCenter(),
                                          glm::vec3(0.0f, 1.0f, 0.0f));
            const auto windowBounds = context.getWindowBounds();
            const glm::mat4 projection = glm::ortho(windowBounds[0], windowBounds[1], windowBounds[2], windowBounds[3],
                                                    0.1f,
                                                    100.0f);

            return projection * view * model;
        }

            void draw() const
            {
            auto& registry = game.getRegistry();
            auto& context = game.getContext();
            for (const auto& entities = registry.view<ecs::TransformComponent, ecs::RenderComponent>(); const auto&
                 entity : entities)
            {
                auto& transform = entities.get<ecs::TransformComponent>(entity);

                if (context.isInVisibleWindow(transform.position))
                {
                    auto& render_component = entities.get<ecs::RenderComponent>(entity);
                    const auto mvpMatrix = calculateMvpMatrix(transform.position, transform.zRotation, transform.scale,
                                                              context);
                    render_component.shader.use();
                    render_component.shader.setMatrix("mvp", mvpMatrix);
                    render_component.shader.setVector("color", render_component.color);
                    if (render_component.texture)
                    {
                        render_component.texture->bind(0); // Bind texture to slot 0
                        render_component.shader.setInt("texture1", 0);
                    }
                    render_component.mesh.draw();
                }
            }
        };
    };
}
