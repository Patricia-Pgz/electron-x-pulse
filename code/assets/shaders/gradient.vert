#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 mvp;

out float vY;

void main() {
    gl_Position = mvp * vec4(aPos, 1.0);
    vY = aPos.y * 0.5 + 0.5;
}

