#version 460 core
in vec2 TexCoord;
uniform sampler2D texture1;
uniform vec4 color;
out vec4 FragColor;

void main() {
    vec4 texColor = texture(texture1, TexCoord);
    FragColor = texColor; //TODO was wenn keine textur -> nur farbe, farbe + textur = texcolor * color, etc.
}