#version 460 core
in vec2 TexCoord;
uniform bool useTexture;
uniform sampler2D texture1;
uniform vec4 color;
out vec4 FragColor;

void main() {
    if (useTexture) {
        vec4 texColor = texture(texture1, TexCoord);
        if (texColor.a < 0.65)
        discard;
        FragColor = texColor;
    } else {
        FragColor = color;
    }

    //TODO farbe + textur = texcolor * color, etc.?
}