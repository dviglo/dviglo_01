// Шейдеры вынесены в отдельный файл для облегчения чтения

static const string SPRITE_VERTEX_SHADER_SRC = R"(
#version 330 core

// Атрибуты вершины
layout (location = 0) in vec2 a_position;    // Позиция левого верхнего угла
layout (location = 1) in vec2 a_size;        // Ширина и высота
layout (location = 2) in vec4 a_color_ul;    // Цвет верхнего левого угла
layout (location = 3) in vec4 a_color_ur;    // Цвет верхнего правого угла
layout (location = 4) in vec4 a_color_dr;
layout (location = 5) in vec4 a_color_dl;
layout (location = 6) in vec2 a_uv_position; // Верхняя левая координата текстуры
layout (location = 7) in vec2 a_uv_size;     // Размер участка текстуры
layout (location = 8) in float a_rotation;   // Поворот
layout (location = 9) in vec2 a_origin;      // 

// Чтобы не вычислять (2 / ширина_экрана, -2 / высота_экрана) для каждого пикселя,
// вычисляется 1 раз на CPU
//uniform vec2 u_scale;

// Передаём в геометрический шейдер
out VS_OUT {
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
} vs_out;

void main()
{
    //gl_Position = vec4(a_position, 0.0, 1.0);
    
    vs_out.position = a_position;
    vs_out.size = a_size/* * u_scale*/;
    vs_out.color_ul = a_color_ul;
    vs_out.color_ur = a_color_ur;
    vs_out.color_dr = a_color_dr;
    vs_out.color_dl = a_color_dl;
    vs_out.uv_position = a_uv_position;
    vs_out.uv_size = a_uv_size;
    vs_out.rotation = a_rotation;
    vs_out.origin = a_origin;
}
)";

static const string SPRITE_GEOMETRY_SHADER_SRC = R"(
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
)";

static const string SPRITE_FRAGMENT_SHADER_SRC = R"(
#version 330 core

in vec4 g_color;
in vec2 g_uv;

uniform sampler2D s_texture;

out vec4 f_color;

void main()
{
    f_color = texture(s_texture, g_uv) * g_color;
}
)";

// =========================================================================

static const string TRIANGLE_VERTEX_SHADER_SRC = R"(
#version 330 core

// Атрибуты вершины
layout (location = 0) in vec2 a_position;
layout (location = 1) in vec4 a_color;

// Чтобы не вычислять (2 / ширина_экрана, -2 / высота_экрана) для каждого пикселя,
// вычисляется 1 раз на CPU
uniform vec2 u_scale;

out vec4 v_color;

void main()
{
    // Это быстрее, чем умножение на матрицу?
    vec2 pos = a_position * u_scale;
    pos += vec2(-1.0, 1.0);
    gl_Position = vec4(pos, 0.0, 1.0);

    v_color = a_color;
}
)";

static const string TRIANGLE_FRAGMENT_SHADER_SRC = R"(
#version 330 core

in vec4 v_color;

out vec4 f_color;

void main()
{
    f_color = v_color;
}
)";
