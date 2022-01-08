// Класс рендерит отдельные глифы (результат в оттенках серого)
// Для понимания кода читайте libs/third-party/000_freetype_01/doc/заметки.md

module;

//#include <ft2build.h> // Необходимо подключить перед другими заголовками библиотеки FreeType
//#include <freetype/freetype.h> // FT_Library
//#include FT_GLYPH_H // <freetype/ftglyph.h>

export module glyph_renderer;

// Модули приложения
import rendered_glyph;

// Модули движка
//import <freetype/freetype.h>; // FT_Library
//#include <freetype/ftstroke.h>

// Стандартные библиотеки
//import <utility>; // std::move

using namespace std;

export class GlyphRenderer
{
private:
    // Библиотека FreeType
    FT_Library library_;

public:
    inline FT_Library library() const
    {
        return library_;
    }

    inline GlyphRenderer()
    {
        FT_Init_FreeType(&library_);
    }

    inline ~GlyphRenderer()
    {
        FT_Done_FreeType(library_);
    }

    RenderedGlyph render_simpe(FT_Face face, FT_UInt glyph_index)
    {
        // Загружаем глиф
        FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);

        // Реднерим глиф
        FT_Glyph glyph;
        FT_Get_Glyph(face->glyph, &glyph);
        FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true);
        FT_BitmapGlyph bitmap_glyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
        RenderedGlyph ret(bitmap_glyph);
        FT_Done_Glyph(glyph);

        return ret;

#if 0
        // Метрики можно извлечь и без рендеринга.
        CalculateMetrics();

        // Реднерим глиф.
        FT_Glyph glyph;
        FT_Get_Glyph(face_->glyph, &glyph);
        FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true);
        FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyph);
        GlyphManipulator glyphManipulator(bitmapGlyph);
        FT_Done_Glyph(glyph);

        // Размываем, если нужно.
        if (blurDistance > 0)
        {
            glyphManipulator.Blur(blurDistance);

            // Так как размытые глифы предназначены для создания теней, то их центры должны
            // совпадать с центрами неразмытых глифов.
            xOffset_ -= blurDistance;
            yOffset_ -= blurDistance;
        }

        SharedPtr<Image> resultImage = ConvertToImage(glyphManipulator, mainColor_);
        PackGlyph(resultImage);
#endif
    }
};
