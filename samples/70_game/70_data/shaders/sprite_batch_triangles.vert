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
