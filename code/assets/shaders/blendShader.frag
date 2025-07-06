#version 460 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float blendFactor; // 0 = only texture0, 1 = only texture1

void main() {
    vec4 color0 = texture(texture0, TexCoord);
    vec4 color1 = texture(texture1, TexCoord);
    FragColor = mix(color0, color1, blendFactor);
}