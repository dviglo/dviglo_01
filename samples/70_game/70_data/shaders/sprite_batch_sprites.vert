#version 330 core

// Атрибуты вершины
layout (location = 0) in vec2 a_position; // Позиция левого верхнего угла
layout (location = 1) in vec2 a_size;     // Ширина и высота
layout (location = 2) in vec4 a_color_ul; // Цвет верхнего левого угла
layout (location = 3) in vec4 a_color_ur; // Цвет верхнего правого угла
layout (location = 4) in vec4 a_color_dr;
layout (location = 5) in vec4 a_color_dl;
layout (location = 6) in vec2 a_uv_position; // Верхняя левая координата текстуры
layout (location = 7) in vec2 a_uv_size; // Размер участка текстуры
layout (location = 8) in float a_rotation; // Поворот
layout (location = 9) in vec2 a_origin; // 



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
