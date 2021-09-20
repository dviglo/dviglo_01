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
