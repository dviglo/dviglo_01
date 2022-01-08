export module camera;

// Модули движка
import <glm/vec2.hpp>;

using namespace glm;

export class Camera
{
    vec2 position{0.f, 0.f};
    float rotation = 0.f; // В радианах против/по? часовой стрелки
};

/*

import <glm/ext/matrix_clip_space.hpp>; // glm::perspective
import <glm/ext/matrix_transform.hpp>; // glm::translate, glm::rotate, glm::scale

mat4 camera(float Translate, vec2 const& Rotate)
{
    mat4 Projection = perspective(pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
    mat4 View = translate(mat4(1.0f), vec3(0.0f, 0.0f, -Translate));
    View = rotate(View, Rotate.y, vec3(-1.0f, 0.0f, 0.0f));
    View = rotate(View, Rotate.x, vec3(0.0f, 1.0f, 0.0f));
    mat4 Model = scale(mat4(1.0f), vec3(0.5f));
    return Projection * View * Model;
}

*/