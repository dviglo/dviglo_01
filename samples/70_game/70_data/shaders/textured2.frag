#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;

uniform vec4 color_uniform;

void main()
{
    //color = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0f);
    color = texture(ourTexture, TexCoord) * color_uniform;
}
