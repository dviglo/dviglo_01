export module dviglo.texture;

// Модули движка
export import <GL/glew.h>;
import dviglo.image;

// Стандартная библиотека
import <string>;
import <memory>; // unique_ptr

using namespace std;

export class dvTexture
{
private:
    // Идентификатор объекта OpenGL
    GLuint gpu_object_name_ = 0;

public:
    dvTexture(const string& file_path)
    {
        unique_ptr<dvImage> image = make_unique<dvImage>(file_path);

        GLenum format;

        if (image->num_components() == 3)
            format = GL_RGB;
        else
            format = GL_RGBA;

        glGenTextures(1, &gpu_object_name_);
        glBindTexture(GL_TEXTURE_2D, gpu_object_name_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width(), image->height(), 0, format, GL_UNSIGNED_BYTE, image->data());
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Запрещаем копировать объект, так как если в одной из копий будет вызван деструктор,
    // все другие объекты будут ссылаться на уничтоженный объект OpenGL
    dvTexture(const dvTexture&) = delete;
    dvTexture& operator=(const dvTexture&) = delete;

    // Но разрешаем перемещение, чтобы было можно хранить текстуру в векторе
    dvTexture(dvTexture&&) = default;
    dvTexture& operator=(dvTexture&&) = default;

    ~dvTexture()
    {
        glDeleteTextures(1, &gpu_object_name_); // Проверка на 0 не нужна
    }

    inline void bind()
    {
        glBindTexture(GL_TEXTURE_2D, gpu_object_name_);
    }

    inline void unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};
