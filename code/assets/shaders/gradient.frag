#version 460 core

in float vY;

out vec4 FragColor;

uniform vec4 topColor;
uniform vec4 bottomColor;

void main() {
    FragColor = mix(bottomColor, topColor, vY);
}
