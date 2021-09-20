// TODO: использовать лог

module;

#define STB_IMAGE_IMPLEMENTATION
//#define STBI_WINDOWS_UTF8
#include <stb_image.h>

export module dviglo.image;

import <cstdint>;
import <string>;

using namespace std;

export class DvImage
{
private:
    int32_t width_;
    int32_t height_;
    int32_t num_components_;
    uint8_t* data_;

public:
    inline int32_t width() const
    {
        return width_;
    }
    
    inline int32_t height() const
    {
        return height_;
    }
    
    inline int32_t num_components() const
    {
        return num_components_;
    }
    
    inline const uint8_t* data() const
    {
        return data_;
    }

private:
    DvImage() = default;

    // Запрещаем копировать объект, размер данных большой, операция тяжёлая
    DvImage(const DvImage&) = delete;
    DvImage& operator=(const DvImage&) = delete;

    // Но разрешаем перемещение, чтобы можно было хранить объекты в векторе
    DvImage(DvImage&& other)
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

    DvImage& operator=(DvImage&& other)
    {
        if (this != &other)
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

        return *this;
    }

public:
    DvImage(const std::string& path)
    {
        data_ = stbi_load(path.c_str(), &width_, &height_, &num_components_, 0);
    }

    ~DvImage()
    {
        stbi_image_free(data_);
    }
};
