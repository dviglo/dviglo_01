#version 330 core

in VS_OUT {
    vec2 position;
    vec2 size;
    vec4 color_ul;
    vec4 color_ur;
    vec4 color_dr;
    vec4 color_dl;
    vec2 uv_position;
    vec2 uv_size;
    float rotation;
    vec2 origin;
} gs_in[];

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec4 g_color;
out vec2 g_uv;

uniform vec2 u_scale; // так как мы смещаем после вращения

void main()
{
    float s = sin(gs_in[0].rotation);
    float c = cos(gs_in[0].rotation);

    // Прямоугольник спрайта, смещенный на минус origin
    vec2 local_min = -gs_in[0].origin;
    vec2 local_max = gs_in[0].size - gs_in[0].origin;

    float m11 = c; float m12 = s; float m13 = gs_in[0].position.x;
    float m21 = -s; float m22 =  c; float m23 = gs_in[0].position.y;

    float minXm11 = local_min.x * m11; // 0
    float minXm21 = local_min.x * m21; // 0
    float maxXm11 = local_max.x * m11; // size.x * c
    float maxXm21 = local_max.x * m21; // size.x * s
    float minYm12 = local_min.y * m12; // 0
    float minYm22 = local_min.y * m22; // 0
    float maxYm12 = local_max.y * m12; // size.y * -s
    float maxYm22 = local_max.y * m22; // size.y * c


    // 1:bottom-left
    g_color = gs_in[0].color_dl;
    g_uv = gs_in[0].uv_position + vec2(0.0, gs_in[0].uv_size.y);
    vec2 pos = vec2(minXm11 + maxYm12 + m13, minXm21 + maxYm22 + m23);
    pos *= u_scale;
    pos += vec2(-1.0, 1.0);
    gl_Position = vec4(pos, 0, 1);
    EmitVertex();   

    // 2:bottom-right
    g_color = gs_in[0].color_dr;
    g_uv = gs_in[0].uv_position + vec2(gs_in[0].uv_size.x, gs_in[0].uv_size.y);
    pos = vec2(maxXm11 + maxYm12 + m13, maxXm21 + maxYm22 + m23);
    pos *= u_scale;
    pos += vec2(-1.0, 1.0);
    gl_Position = vec4(pos, 0, 1);
    EmitVertex();

    // 3:top-left
    g_color = gs_in[0].color_ul;
    g_uv = gs_in[0].uv_position + vec2(0.0, 0.0);
    pos = vec2(minXm11 + minYm12 + m13, minXm21 + minYm22 + m23);
    pos *= u_scale;
    pos += vec2(-1.0, 1.0);
    gl_Position = vec4(pos, 0, 1);
    EmitVertex();

    // 4:top-right
    g_color = gs_in[0].color_ur;
    g_uv = gs_in[0].uv_position + vec2(gs_in[0].uv_size.x, 0.0);
    pos = vec2(maxXm11 + minYm12 + m13, maxXm21 + minYm22 + m23);
    pos *= u_scale;
    pos += vec2(-1.0, 1.0);
    gl_Position = vec4(pos, 0, 1);
    EmitVertex();

    EndPrimitive();
}
