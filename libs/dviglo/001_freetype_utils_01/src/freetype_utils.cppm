module;

#include <ft2build.h> // Необходимо подключить перед другими заголовками библиотеки FreeType
#include FT_IMAGE_H // <freetype/ftimage.h>, FT_Pos
#include <freetype/internal/ftobjs.h> // FT_PIX_ROUND

export module dviglo.freetype_utils;

// Модули движка
import dviglo.base; // i32

// Преобразует число в формате 26.6 в обычный i32 с округлением к ближайшему целому
export inline i32 dv_26dot6_to_i32(FT_Pos value)
{
    // https://www.freetype.org/freetype2/docs/glyphs/glyphs-6.html#section-3
    return FT_PIX_ROUND(value) >> 6;
}

/*
export inline int ascender(FT_Face face)
{
    return dv_26dot6_to_i32(face->size->metrics.ascender);
}
*/
