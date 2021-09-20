export module dviglo.texture;

// Модули движка
export import <GL/glew.h>;
import dviglo.image;

// Стандартная библиотека
import <string>;
import <memory>; // unique_ptr

using namespace std;

export class DvTexture
{
private:
    // Идентификатор объекта OpenGL
    GLuint gpu_object_name_;

public:
    inline GLuint gpu_object_name() const
    {
        return gpu_object_name_;
    }

    DvTexture()
    {
        gpu_object_name_ = 0;
    }

    DvTexture(const string& file_path)
    {
        DvImage image(file_path);

        GLenum format;

        if (image.num_components() == 3)
            format = GL_RGB;
        else
            format = GL_RGBA;

        glGenTextures(1, &gpu_object_name_);
        glBindTexture(GL_TEXTURE_2D, gpu_object_name_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, format, GL_UNSIGNED_BYTE, image.data());
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ~DvTexture()
    {
        glDeleteTextures(1, &gpu_object_name_); // Проверка на 0 не нужна
        gpu_object_name_ = 0;
    }

    // Запрещаем копировать объект, так как если в одной из копий будет вызван деструктор,
    // все другие объекты будут ссылаться на уничтоженный объект OpenGL
    DvTexture(const DvTexture&) = delete;
    DvTexture& operator=(const DvTexture&) = delete;

    // Но разрешаем перемещение, чтобы можно было хранить объекты в векторе
    DvTexture(DvTexture&& other)
    {
        gpu_object_name_ = other.gpu_object_name_;
        other.gpu_object_name_ = 0;
    }

    DvTexture& operator=(DvTexture&& other)
    {
        if (this != &other)
        {
            gpu_object_name_ = other.gpu_object_name_;
            other.gpu_object_name_ = 0;
        }

        return *this;
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
