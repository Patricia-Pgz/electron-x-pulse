#include "Entity.h"
#include "../Game.h"

namespace gl3 {
    Entity::Entity(Shader shader, Mesh mesh, glm::vec3 position, float zRotation, glm::vec3 scale, glm::vec4 color) :
            shader(std::move(shader)),
            mesh(std::move(mesh)),
            position(position),
            zRotation(zRotation),
            scale(scale),
            color(color) {}

    void Entity::draw(Game *game) {
        auto mvpMatrix = game->calculateMvpMatrix(position, zRotation, scale);
        shader.use();
        shader.setMatrix("mvp", mvpMatrix);
        shader.setVector("color", color);
        mesh.draw();
    }
}