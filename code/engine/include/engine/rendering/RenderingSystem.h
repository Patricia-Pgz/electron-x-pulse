#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace gl3::engine::rendering
{
    class RenderingSystem
    {
    public:
        static glm::mat4 calculateMvpMatrix(glm::vec3& position, float& zRotationInDegrees, glm::vec3 scale, gl3::engine::context::Context& context)
        {
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, position);
            model = glm::scale(model, scale);
            model = glm::rotate(model, glm::radians(zRotationInDegrees), glm::vec3(0.0f, 0.0f, 1.0f));

            glm::mat4 view = glm::lookAt(context.getCameraPos(),
                                         context.getCameraCenter(),
                                         glm::vec3(0.0f, 1.0f, 0.0f));
            auto windowBounds = context.getWindowBounds();
            glm::mat4 projection = glm::ortho(windowBounds[0], windowBounds[1], windowBounds[2], windowBounds[3], 0.1f,
                                              100.0f);

            return projection * view * model;
        }

           static void draw(entt::registry& registry, gl3::engine::context::Context& context){
            auto entities = registry.view<engine::ecs::TransformComponent, engine::ecs::RenderComponent>();
            for (const auto& entity : entities)
            {
                auto& transform = entities.get<engine::ecs::TransformComponent>(entity);

                if (context.isInVisibleWindow(transform.position)){
                    auto& physics_component = entities.get<engine::ecs::RenderComponent>(entity);
                    const auto mvpMatrix = calculateMvpMatrix(transform.position, transform.zRotation, transform.scale, context);
                    physics_component.shader.use();
                    physics_component.shader.setMatrix("mvp", mvpMatrix);
                    physics_component.shader.setVector("color", physics_component.color);
                    physics_component.mesh.draw();
                }

            }
          };



    };
}