// TODO: использовать лог

module;

#define STB_IMAGE_IMPLEMENTATION
//#define STBI_WINDOWS_UTF8
#include <stb_image.h>

export module dviglo.image;

// Модули движка
import dviglo.primitive_types; // i32, u8

// Стандартная библиотека
import <string>;

using namespace std;

export class DvImage
{
private:
    i32 width_;
    i32 height_;
    i32 num_components_;
    u8* data_;

    inline void clean_fields()
    {
        width_ = 0;
        height_ = 0;
        num_components_ = 0;
        data_ = 0;
    }

public:
    inline i32 width() const
    {
        return width_;
    }
    
    inline i32 height() const
    {
        return height_;
    }
    
    inline i32 num_components() const
    {
        return num_components_;
    }
    
    inline const u8* data() const
    {
        return data_;
    }

    inline DvImage()
    {
        clean_fields();
    }

    DvImage(i32 width, i32 height, i32 num_components)
    {
        assert(width > 0 && height > 0 && (num_components == 3 || num_components == 4));

        width_ = width;
        height_ = height;
        num_components_ = num_components;
        data_ = (u8*)stbi__malloc(width * height * num_components);
    }

    DvImage(const string& path)
    {
        data_ = stbi_load(path.c_str(), &width_, &height_, &num_components_, 0);
    }

    ~DvImage()
    {
        stbi_image_free(data_);
        clean_fields();
    }

    // Запрещаем копировать объект, размер данных большой, операция тяжёлая
    DvImage(const DvImage&) = delete;
    DvImage& operator=(const DvImage&) = delete;

    // Но разрешаем перемещение, чтобы можно было хранить объекты в векторе
    DvImage(DvImage&& other) noexcept
    {
        width_ = other.width_;
        height_ = other.height_;
        num_components_ = other.num_components_;
        data_ = other.data_;

        other.width_ = 0;
        other.height_ = 0;
        other.num_components_ = 0;
        other.data_ = nullptr;
    }

    DvImage& operator=(DvImage&& other) noexcept
    {
        if (this != &other)
        {
            stbi_image_free(data_);

            width_ = other.width_;
            height_ = other.height_;
            num_components_ = other.num_components_;
            data_ = other.data_;

            other.width_ = 0;
            other.height_ = 0;
            other.num_components_ = 0;
            other.data_ = nullptr;
        }

        return *this;
    }

    // Устанавливает цвет пикселя. Требует чтобы изображение состояло из abgr
    void pixel(i32 x, i32 y, u32 color)
    {
        assert(x >= 0 && x < width() && y >= 0 && y < height() && num_components() == 4);

        u32* ptr = (u32*)(data_);
        ptr[y * width() + x] = color;
    }

    // Устанавливает цвет пикселя. Требует чтобы изображение состояло из abgr
    void clear(u32 color)
    {
        assert(num_components() == 4);

        u32* ptr = (u32*)(data_);

        for (int i = 0; i < width() * height(); ++i)
            ptr[i] = color;
    }
};
