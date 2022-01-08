/*
    Библиотека FreeType рендерит глиф в FT_BitmapGlyph, а потом FT_BitmapGlyph преобразуется в
    RenderedGlyph, чтобы произвести дополнительные манипуляции с изображением
*/

module;

#include <ft2build.h> // Необходимо подключить перед другими заголовками библиотеки FreeType

export module rendered_glyph;

// Модули движка
export import dviglo.array_2d; // DvArray2D
export import dviglo.image; // DvImage
export import <freetype/ftglyph.h>; // <freetype/ftglyph.h>

// Стандартные библиотеки
import <algorithm>; // clamp
import <cassert>; // assert

using namespace std;

/*
static inline float to_float(u8 color)
{
    return color / 255.f;
}

static inline u8 to_byte(float color)
{
    return static_cast<u8>(round(clamp(color, 0.0f, 1.0f) * 255.0f));
}
*/

export struct RenderedGlyph
{
    // Grayscale пиксели изображения в диапазоне 0.0 - 1.0
    DvArray2D<float> pixels;

    // Проверяет, что координаты не выходят за границы изображения
    inline bool is_inside(i32 x, i32 y)
    {
        return x >= 0 && x < pixels.width() && y >= 0 && y < pixels.height();
    }

    inline void pixel(i32 x, i32 y, float gray)
    {
        assert(is_inside(x, y));
        assert(gray >= 0.f && gray <= 1.f);
        pixels.set(x, y, gray);
    }

    inline float pixel(i32 x, i32 y)
    {
        assert(is_inside(x, y));
        return pixels.get(x, y);
    }

    // Конструктор копирует данные из FT_BitmapGlyph (FT_BitmapGlyph - это указатель)
    RenderedGlyph(const FT_BitmapGlyph bitmap_glyph)
        : pixels(bitmap_glyph->bitmap.width, bitmap_glyph->bitmap.rows)
    {
        if (bitmap_glyph->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
        {
            // В монохромном изображении один пиксель занимает 1 бит (не байт).
            // pitch - это число байт, занимаемых одной линией изображения
            for (i32 y = 0; y < pixels.height(); ++y)
            {
                unsigned char* src_line = bitmap_glyph->bitmap.buffer + bitmap_glyph->bitmap.pitch * y;

                for (i32 x = 0; x < pixels.width(); ++x)
                {
                    // 1) В одном байте умещается 8 пикселей. x >> 3 эквивалентно делению
                    //    на 8 (0b1000 превращается в 0b1). Так мы получаем байт, в котором находится пиксель
                    // 2) x & 7 - это остаток от деления на 8. Допустим x = 12 = 0b1100.
                    //    0b1100 & 0b0111 = 0b100 = 4. Так мы получаем номер бита внутри байта
                    // 3) 0x80 == 0b10000000. Единица внутри этого числа сдвигается на номер бита
                    //    и побитовой операцией определяется значение бита
                    float gray = (src_line[x >> 3] & (0x80 >> (x & 7))) ? 1.f : 0.f;
                    pixels.set(x, y, gray);
                }
            }
        }
        else
        {
            // В grayscale изображении каждый пиксель занимает один байт,
            // а значит pitch эквивалентен width
            for (i32 i = 0; i < pixels.width() * pixels.height(); ++i)
                pixels.data()[i] = bitmap_glyph->bitmap.buffer[i] / 255.f;
        }
    }

    // Некоторые эффекты требуют увеличения размера битмапа.
    // Битмап расширяется в каждую сторону на half_dist, то есть ширина и высота увеличатся на half_dist * 2.
    // Старое изображение располагается в центре нового
    void expand(i32 half_dist)
    {
        if (!half_dist)
            return;

        DvArray2D<float> new_pixels(pixels.width() + half_dist * 2, pixels.height() + half_dist * 2);

        // Старое изображение помещается в центр нового, а края нового изображения - чёрные
        new_pixels.fill(0.f);

        for (i32 x = 0; x < pixels.width(); ++x)
        {
            i32 new_x = x + half_dist;

            for (i32 y = 0; y < pixels.height(); ++y)
            {
                i32 new_y = y + half_dist;
                new_pixels.set(new_x, new_y, pixels.get(x, y));
            }
        }

        pixels = std::move(new_pixels);
    }

    // Размазывает каждый пиксель на radius в каждую сторону.
    // Размеры итогового изображения увеличатся на radius * 2 по вертикали и по горизонтали
    void blur(i32 radius)
    {
        // Пробовал все три алгоритма с http://www.gamedev.ru/code/articles/blur .
        // Обычное усреднение (первый метод) выглядит уродливо, изображение как будто двоится.
        // Гауссово размытие (третий метод) выглядит красиво, но интенсивность пикселей очень
        // быстро и нелинейно затухает с расстоянием. Бо'льшая часть радиуса размытия почти
        // полностью прозрачна и не видна. Приходится нереально накручивать радиус размытия,
        // чтобы хоть немного увеличить видимую площадь эффекта. А это бьет по производительности.
        // Поэтому используется треугольный закон распределения (второй метод)

        if (radius == 0)
            return;

        expand(radius);

        DvArray2D<float> temp(pixels.width(), pixels.height());

        // Пиксель на каждом проходе размывается в две стороны, и получается отрезок
        // длиной radius + 1 + radius.
        // У крайних пикселей вес самый маленький и равен 1.
        // У центрального пикселя вес самый большой и равен radius + 1.
        // Вес соседних пикселей отличается на 1 (убывает линейно).
        // Сумму весов всех пикселей в этом отрезке можно определить сразу
        i32 total_weight = (radius + 1) * (radius + 1);

        // Размываем по вертикали и сохраняем результат в temp
        for (i32 x = 0; x < pixels.width(); ++x)
        {
            for (i32 y = 0; y < pixels.height(); ++y)
            {
                // Сразу записываем вклад центрального пикселя.
                // Его вес равен radius + 1
                float value = pixel(x, y) * (radius + 1);

                i32 dist = 1;
                while (dist <= radius)
                {
                    i32 weight = 1 + radius - dist;

                    // Пиксель вне изображения черный, ноль можно не плюсовать.
                    // Так что тут все корректно
                    if (is_inside(x, y + dist))
                        value += pixel(x, y + dist) * weight;

                    if (is_inside(x, y - dist))
                        value += pixel(x, y - dist) * weight;

                    ++dist;
                }

                // Сумму нужно поделить на общий вес, иначе изменится яркость изображения
                value /= total_weight;

                // Как бы не вышло больше 1 из-за погрешности вычислений
                value = clamp(value, 0.f, 1.f);

                temp.set(x, y, value);
            }
        }

        // Размываем по горизонтали и сохраняем результат назад в pixels
        for (i32 x = 0; x < temp.width(); ++x)
        {
            for (i32 y = 0; y < temp.height(); ++y)
            {
                float value = temp.get(x, y) * (radius + 1);
                i32 dist = 1;

                while (dist <= radius)
                {
                    i32 weight = 1 + radius - dist;

                    if (is_inside(x + dist, y))
                        value += temp.get(x + dist, y) * weight;

                    if (is_inside(x - dist, y))
                        value += temp.get(x - dist, y) * weight;

                    ++dist;
                }

                pixel(x, y, clamp(value / total_weight, 0.f, 1.f));
            }
        }
    }

    // Преобразует grayscale изображение в цветное
    DvImage to_image(u32 color)
    {
        DvImage ret(pixels.width(), pixels.height(), 4);

        for (i32 y = 0; y < pixels.height(); ++y)
        {
            for (i32 x = 0; x < pixels.width(); ++x)
            {
                float mask = pixel(x, y);

                u32 rgb = color & 0x00ffffff;
                u32 a = color >> 24;
                a = (u32)(a * mask);
                u32 result_color = rgb | (a << 24);

                ret.pixel(x, y, result_color);
            }
        }

        return ret;
    }
};
