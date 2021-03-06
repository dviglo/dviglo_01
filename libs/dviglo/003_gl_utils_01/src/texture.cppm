export module dviglo.texture;

// Модули движка
export import dviglo.image; // DvImage
export import <GL/glew.h>;

// Стандартная библиотека
export import <string>;

using namespace std;

export class DvTexture
{
private:
    // Идентификатор объекта OpenGL
    GLuint gpu_object_name_;

    GLsizei width_;
    GLsizei height_;

public:
    inline GLuint gpu_object_name() const
    {
        return gpu_object_name_;
    }

    inline GLsizei width() const
    {
        return width_;
    }

    inline GLsizei height() const
    {
        return height_;
    }

    inline DvTexture()
    {
        gpu_object_name_ = 0;
        width_ = 0;
        height_ = 0;
    }

    DvTexture(const DvImage& image)
    {
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

        width_ = image.width();
        height_ = image.height();
    }

    DvTexture(const string& file_path)
        : DvTexture(DvImage(file_path))
    {
    }

    inline ~DvTexture()
    {
        glDeleteTextures(1, &gpu_object_name_); // Проверка на 0 не нужна
        gpu_object_name_ = 0;
    }

    // Запрещаем копировать объект, так как если в одной из копий будет вызван деструктор,
    // все другие объекты будут ссылаться на уничтоженный объект OpenGL
    DvTexture(const DvTexture&) = delete;
    DvTexture& operator=(const DvTexture&) = delete;

    // Но разрешаем перемещение, чтобы можно было хранить объекты в векторе
    inline DvTexture(DvTexture&& other) noexcept
    {
        gpu_object_name_ = other.gpu_object_name_;
        width_ = other.width_;
        height_ = other.height_;
        other.gpu_object_name_ = 0;
    }

    inline DvTexture& operator=(DvTexture&& other) noexcept
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
