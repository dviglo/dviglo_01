/*
    Операции с выделенным куском памяти как с 2D - массивом
*/

export module dviglo.array_2d;

// Модули движка
import dviglo.base; // i32

// Стандартная библиотека
import <algorithm>; // copy
import <cassert>; // assert

using namespace std;

export
template<typename T>
class DvArray2D
{
private:
    i32 width_;
    i32 height_;
    T* data_;

    inline void clean_fields()
    {
        width_ = 0;
        height_ = 0;
        data_ = nullptr;
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
    
    inline T* data() const
    {
        return data_;
    }

    inline DvArray2D(i32 width, i32 height)
    {
        assert(width > 0 && height > 0);
        width_ = width;
        height_ = height;
        data_ = new T[width * height];
    }

    inline ~DvArray2D()
    {
        delete[] data_;
        clean_fields();
    }

    // Конструктор копирования
    // b(a);
    inline DvArray2D(const DvArray2D& other)
    {
        width_ = other.width_;
        height_ = other.height_;
        data_ = new T[width_ * height_];
        copy(other.data_, other.data_ + width_ * height_, data_);
    }

    // Оператор копирования
    // b = a;
    inline DvArray2D& operator=(const DvArray2D& other)
    {
        if (this != &other)
        {
            width_ = other.width_;
            height_ = other.height_;
            delete[] data_;
            data_ = new T[width_ * height_];
            copy(other.data_, other.data_ + width_ * height_, data_);
        }
        
        return *this;
    }

    // Конструктор перемещения
    // b(move(a));
    inline DvArray2D(DvArray2D&& other) noexcept
    {
        width_ = other.width_;
        height_ = other.height_;
        data_ = other.data_;
        other.clean_fields();
    }

    // Оператор перемещения
    // b = move(a);
    inline DvArray2D& operator=(DvArray2D&& other) noexcept
    {
        if (this != &other)
        {
            width_ = other.width_;
            height_ = other.height_;
            delete[] data_;
            data_ = other.data_;
            other.clean_fields();
        }

        return *this;
    }

    inline T& get(i32 x, i32 y) const
    {
        assert(x >= 0 && x < width_ && y >= 0 && y < height_);
        return data_[y * width_ + x];
    }

    inline void set(i32 x, i32 y, const T& value)
    {
        assert(x >= 0 && x < width_ && y >= 0 && y < height_);
        data_[y * width_ + x] = value;
    }

    inline void fill(const T& value)
    {
        for (i32 i = 0; i < width_ * height_; ++i)
            data_[i] = value;
    }
};
