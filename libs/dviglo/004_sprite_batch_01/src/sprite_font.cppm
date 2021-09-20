export module sprite_font;

// Модули движка
export import dviglo.texture; // dvTexture
import <pugixml.hpp>;
import dviglo.log; // dvLog
import dviglo.path; // dv_path

// Стандартная библиотека
import <unordered_map>;

using namespace pugi;
using namespace std;

export struct Glyph
{
    // Горизонтальная позиция в текстуре
    i16 x = 0;

    // Вертикальаня позиция в текстуре
    i16 y = 0;

    // Ширина
    i16 width = 0;
    
    // Высота
    i16 height = 0;
    
    // Смещени от origin (левый верхний угол) по горизонтали при рендеринге
    i16 offset_x = 0;

    // Смещение от origin (левый верхний угол) по вертикали при рендеринге
    i16 offset_y = 0;

    // Расстояние между origin (левый верхний угол) текущего глифа и origin следующего глифа при рендеринге
    i16 advance_x = 0;

    // Номер текстуры
    i16 page = numeric_limits<i16>::max();
};

export class DvSpriteFont
{
private:
    string face_; // Название исходного шрифта (из которого был сгенерирован растровый шрифт)
    i32 size_ = 0; // Размер исходного шрифта
    i32 line_height_ = 0; // Высота растрового шрифта
    vector<dvTexture> textures_; // Текстурные атласы с символами
    unordered_map<u32, Glyph> glyphs_; // кодовая позиция -> изображение

public:
    inline dvTexture& texture(size_t index)
    {
        return textures_[index];
    }

    inline const Glyph& glyph(u32 code_point)
    {
        return glyphs_[code_point];
    }

    DvSpriteFont(const string& file_path)
    {
        xml_document doc;
        xml_parse_result result = doc.load_file(file_path.c_str());
        if (!result)
        {
            LOG().write_error("DvSpriteFont::DvSpriteFont(file_path): !result | file_path = " + file_path);
            return;
        }

        xml_node root_node = doc.first_child();
        if (root_node.name() != string("font"))
        {
            LOG().write_error("DvSpriteFont::DvSpriteFont(file_path): root_node.name() != string(\"font\") | file_path = " + file_path);
            return;
        }

        xml_node pages_node = root_node.child("pages");
        if (!pages_node)
        {
            LOG().write_error("DvSpriteFont::DvSpriteFont(file_path): !pages_node | file_path = " + file_path);
            return;
        }

        xml_node info_node = root_node.child("info");
        if (info_node)
            size_ = stoi(info_node.attribute("size").value());

        xml_node common_node = root_node.child("common");
        line_height_ = common_node.attribute("lineHeight").as_int();
        i32 pages = common_node.attribute("pages").as_int();
        textures_.reserve(pages);

        string directory_path = dv_path::get_directory_path(file_path);

        xml_node page_node = pages_node.child("page");
        for (i32 i = 0; i < pages; ++i)
        {
            if (!page_node)
            {
                LOG().write_error("DvSpriteFont::DvSpriteFont(file_path): !page_node | file_path = " + file_path);
                return;
            }

            string image_file_name = page_node.attribute("file").as_string();
            string image_file_path = directory_path + '/' + image_file_name;
            textures_.emplace_back(image_file_path);

            page_node = page_node.next_sibling();
        }

        for (xml_node char_node : root_node.child("chars"))
        {
            u32 id = char_node.attribute("id").as_uint();
            Glyph glyph;
            glyph.x = (i16)char_node.attribute("x").as_uint();
            glyph.y = (i16)char_node.attribute("y").as_uint();
            glyph.width = (i16)char_node.attribute("width").as_uint();
            glyph.height = (i16)char_node.attribute("height").as_uint();
            glyph.offset_x = (i16)char_node.attribute("xoffset").as_uint();
            glyph.offset_y = (i16)char_node.attribute("yoffset").as_uint();
            glyph.advance_x = (i16)char_node.attribute("xadvance").as_uint();
            glyph.page = (i16)char_node.attribute("page").as_uint();
            glyphs_[id] = glyph;
        }

        // TODO керинг не загружается
    }
};
