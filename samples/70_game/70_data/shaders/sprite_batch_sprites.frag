#version 330 core

in vec4 g_color;
in vec2 g_uv;

uniform sampler2D s_texture;

out vec4 f_color;

void main()
{
    f_color = texture(s_texture, g_uv) * g_color;
}
