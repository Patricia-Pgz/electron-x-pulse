#version 460 core
in vec2 TexCoord;
uniform bool useTexture;
uniform sampler2D texture1;
uniform vec2 uvOffset;
uniform vec4 color;

out vec4 FragColor;

void main() {
    vec4 finalColor;

    if (useTexture) {
        // Apply UV parallax offset
        vec2 offsetUV = TexCoord + uvOffset;
        vec4 texColor = texture(texture1, offsetUV);

        // Discard transparent fragments
        if (texColor.a < 0.65)
        discard;

        finalColor = texColor;
    } else {
        // Use plain vertex color if no texture
        finalColor = color;
    }

    FragColor = finalColor;
}