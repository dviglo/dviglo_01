export module sprite_batch;

// Модули приложения
export import sprite_font; // DvSpriteFont

// Модули движка
import dviglo.base; // u32
import dviglo.shader_program;
import dviglo.std_string_utils; // next_code_point()

using namespace glm;
using namespace std;

// Так как участки памяти копируются напрямую, то убеждаемся, что структура имеет ожидаемый размер
static_assert(sizeof(vec2) == 2 * 4);

#include "shaders.inl"

export class DvSpriteBatch
{

// ============================ Пакетный рендеринг треугольников ============================

private:

    // Атрибуты вершин треугольников
    struct TVertex
    {
        vec3 position;

        // На стороне CPU цвет - u32 (0xAABBGGRR), который интерпретируется как четыре отдельных байта.
        // На стороне GPU цвет автоматически преобразуется в четыре float
        u32 color;
    };

    // Максимальное число треугольников в порции
    static inline const u32 MAX_TRIANGLES_IN_PORTION = 600;

    // Число вершин в треугольнике
    static inline const u32 VERTICES_PER_TRIANGLE = 3;

    // Текущая порция треугольников
    TVertex t_vertices_[MAX_TRIANGLES_IN_PORTION * VERTICES_PER_TRIANGLE];

    // Число вершин в массиве t_vertices_
    u32 t_num_vertices_ = 0;

    // Шейдерная программа для рендеринга треугольников
    dvShaderProgram t_shader_program_{TRIANGLE_VERTEX_SHADER_SRC, TRIANGLE_FRAGMENT_SHADER_SRC};

    // Vertex Buffer Object - объект на GPU, хранящий вершины треугольников
    GLuint t_vbo_name_;

    // Vertex Array Object - объект на GPU, хранящий VBO + данные о формате вершин
    GLuint t_vao_name_;

public:

    // Данные для функции add_triangle().
    // Заполняем заранее выделенную память, вместо передачи кучи аргументов в функцию
    struct
    {
        TVertex v0, v1, v2;
    } triangle;

    // Добавляет 3 вершины в массив t_vertices_. Вызывает flush(), если массив полон.
    // Перед вызовом этой функции необходимо заполнить структуру triangle
    void add_triangle()
    {
        memcpy(t_vertices_ + t_num_vertices_, &triangle, sizeof(triangle));
        t_num_vertices_ += VERTICES_PER_TRIANGLE;

        // Если после добавления вершин мы заполнили массив до предела, то рендерим порцию
        if (t_num_vertices_ == MAX_TRIANGLES_IN_PORTION * VERTICES_PER_TRIANGLE)
            flush();
    }

    // Указывает цвет для следующих треугольников (0xAABBGGRR)
    void shape_color(u32 color)
    {
        triangle.v0.color = color;
        triangle.v1.color = color;
        triangle.v2.color = color;
    }

// ============================ Пакетный рендеринг спрайтов ============================

private:

    // Данные спрайтов, хранящиеся в вершинном буфере GPU.
    // На GPU передются позиция верхнего левого угла и размер спрайта, а геометрический шейдер создаёт вершины спрайта.
    // Это сделано для того, чтобы вращение и масштабирование спрайтов выполнялось на GPU, а не на CPU.
    // Даёт ли это прирост производительности - не тестировалось
    struct Sprite
    {
        // Позиция верхнего левого угла спрайта
        vec2 position;

        // Ширина и высота спрайта
        vec2 size;

        // Цвет верхнего левого угла
        u32 color_ul;

        // Цвет нижнего левого угла
        u32 color_dl;

        // Цвет нижнего правого угла
        u32 color_dr;

        // Цвет верхнего правого угла
        u32 color_ur;

        // Верхняя левая координата текстуры
        vec2 uv_position;

        // Размер участка текстуры
        vec2 uv_size;

        // Поворот спрайта в радианах против часовой стрелки
        float rotation;

        // Центр вращения спрайта
        vec2 origin;
    };

    // Максимальное число спрайтов в порции
    static inline const u32 MAX_SPRITES_IN_PORTION = 500;

    // Текущая порция спрайтов
    Sprite sprites_[MAX_SPRITES_IN_PORTION];

    // Число вершин в массиве sprites_
    u32 num_sprites_ = 0;

    // Шейдерная программа для рендеринга спрайтов
    dvShaderProgram s_shader_program_{SPRITE_VERTEX_SHADER_SRC, SPRITE_FRAGMENT_SHADER_SRC, SPRITE_GEOMETRY_SHADER_SRC};

    // Vertex Buffer Object - объект на GPU, хранящий спрайты
    GLuint s_vbo_name_;

    // Vertex Array Object - объект на GPU, хранящий VBO + данные об атрибутах
    GLuint s_vao_name_;

    // Текущая текстура
    dvTexture* texture_ = nullptr;

public:

    // Возвращает текущую текстуру
    inline dvTexture* texture() const
    {
        return texture_;
    }

    // Устанавливает текущую текстуру. Если ссылка отличается от прежнего значения и происходит накопление спрайтов, то вызывается flush()
    inline void texture(dvTexture* value)
    {
        if (texture_ != value && num_sprites_)
            flush();

        texture_ = value;
    }

    // Данные для функции add_sprite().
    // Заполняем заранее выделенную память, вместо передачи кучи аргументов в функцию
    Sprite sprite;

    void add_sprite()
    {
        memcpy(sprites_ + num_sprites_, &sprite, sizeof(sprite));
        ++num_sprites_;

        // Если после добавления спрайта мы заполнили массив до предела, то рендерим порцию
        if (num_sprites_ == MAX_SPRITES_IN_PORTION)
            flush();
    }

    // Указывает цвет для следующих спрайтов (0xAABBGGRR)
    void sprite_color(u32 color)
    {
        sprite.color_ul = color;
        sprite.color_dl = color;
        sprite.color_dr = color;
        sprite.color_ur = color;
    }

// ============================ Общее ============================

private:

    bool alpha_blending_ = false;

public:

    DvSpriteBatch()
    {
        // ================================= Треугольники =================================

        glGenVertexArrays(1, &t_vao_name_);
        glBindVertexArray(t_vao_name_);

        glGenBuffers(1, &t_vbo_name_);
        glBindBuffer(GL_ARRAY_BUFFER, t_vbo_name_);

        // Просто выделяем память на GPU без копирования данных
        glBufferData(GL_ARRAY_BUFFER, sizeof(t_vertices_), nullptr, GL_DYNAMIC_DRAW);

        // Атрибут с координатами
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TVertex), (const void*)0);
        glEnableVertexAttribArray(0);

        // Атрибут с цветом.
        // На стороне CPU цвет - u32 (0xAABBGGRR), который интерпретируется как четыре отдельных байта.
        // На стороне GPU цвет автоматически преобразуется в четыре float
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(TVertex), (const void*)sizeof(vec2));
        glEnableVertexAttribArray(1);

        // По умолчанию белый цвет
        shape_color(0xFFFFFFFF);

        // Отвязываем VAO, чтобы не испортить
        glBindVertexArray(0);

        // ================================= Спрайты =================================

        glGenVertexArrays(1, &s_vao_name_);
        glBindVertexArray(s_vao_name_);

        glGenBuffers(1, &s_vbo_name_);
        glBindBuffer(GL_ARRAY_BUFFER, s_vbo_name_);

        // Просто выделяем память на GPU без копирования данных
        glBufferData(GL_ARRAY_BUFFER, sizeof(sprites_), nullptr, GL_DYNAMIC_DRAW);

        // sprite.position
        size_t offset = 0;
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Sprite), (const void*)offset);
        glEnableVertexAttribArray(0);
        offset += sizeof(vec2); // Вычисляем смещение для следующего атрибута

        // sprite.size
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Sprite), (const void*)offset);
        glEnableVertexAttribArray(1);
        offset += sizeof(vec2);

        // sprite.color_ul
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Sprite), (const void*)offset);
        glEnableVertexAttribArray(2);
        offset += sizeof(u32);

        // sprite.color_ur
        glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Sprite), (const void*)offset);
        glEnableVertexAttribArray(3);
        offset += sizeof(u32);

        // sprite.color_dr
        glVertexAttribPointer(4, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Sprite), (const void*)offset);
        glEnableVertexAttribArray(4);
        offset += sizeof(u32);

        // sprite.color_dl
        glVertexAttribPointer(5, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Sprite), (const void*)offset);
        glEnableVertexAttribArray(5);
        offset += sizeof(u32);

        // sprite.uv_position
        glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(Sprite), (const void*)offset);
        glEnableVertexAttribArray(6);
        offset += sizeof(vec2);

        // sprite.uv_size
        glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(Sprite), (const void*)offset);
        glEnableVertexAttribArray(7);
        offset += sizeof(vec2);

        // sprite.rotation
        glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(Sprite), (const void*)offset);
        glEnableVertexAttribArray(8);
        offset += sizeof(float);

        // sprite.origin
        glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, sizeof(Sprite), (const void*)offset);
        glEnableVertexAttribArray(9);
        offset += sizeof(vec2);

        // По умолчанию белый цвет
        sprite_color(0xFFFFFFFF);

        // Отвязываем настроенный VAO, чтобы не испортить
        glBindVertexArray(0);
    }

    ~DvSpriteBatch()
    {
        glDeleteBuffers(1, &t_vbo_name_);
        glDeleteVertexArrays(1, &t_vao_name_);
        glDeleteBuffers(1, &s_vbo_name_);
        glDeleteVertexArrays(1, &s_vao_name_);
    }

    inline bool alpha_blending() const
    {
        return alpha_blending_;
    }

    void alpha_blending(bool value)
    {
        // Всегда вызваем flush(), так как текущее состояние не хранится и вообще может быть изменено вне класса
        flush();

        if (value)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBlendEquation(GL_FUNC_ADD);
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }

    ivec2 screen_size{ 800, 600 };

    void flush()
    {
        if (t_num_vertices_)
        {
            t_shader_program_.use();
            t_shader_program_.set("u_scale", vec2(2.0f / screen_size.x, -2.0f / screen_size.y));

            // Копируем накопленную геометрию в память видеокарты
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TVertex) * t_num_vertices_, t_vertices_);

            // И отрисовываем её
            glBindVertexArray(t_vao_name_);
            glDrawArrays(GL_TRIANGLES, 0, t_num_vertices_);
            glBindVertexArray(0);

            // Начинаем новую порцию
            t_num_vertices_ = 0;
        }

        if (num_sprites_)
        {
            texture_->bind();

            s_shader_program_.use();
            s_shader_program_.set("u_scale", vec2(2.0f / screen_size.x, -2.0f / screen_size.y));

            // Копируем накопленную геометрию в память видеокарты
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Sprite) * num_sprites_, sprites_);

            // И отрисовываем её
            glBindVertexArray(s_vao_name_);
            glDrawArrays(GL_POINTS, 0, num_sprites_);
            glBindVertexArray(0);

            // Начинаем новую порцию
            num_sprites_ = 0;
        }
    }

// ============================ Используем пакетный рендерер спарайтов ============================

    void draw_string(const string& text, DvSpriteFont* font, const vec2& position, u32 color = 0xFFFFFFFF, float rotation = 0.0f/*, float fontSize, const Vector2& position, u32 color = 0xFFFFFFFF,
        float rotation = 0.0f, const Vector2& origin = Vector2::ZERO, const Vector2& scale = Vector2::ONE, FlipMode flipMode = FlipMode::NONE*/)
    {
        texture(&font->texture(0));

        sprite_color(color);

        float advance_x_sum = 0.f;

        size_t offset = 0;
        while (offset < text.length())
        {
            u32 code_point = next_code_point(text, offset);
            const Glyph& glyph = font->glyph(code_point);

            sprite.position.y = position.y/* + glyph.offset_y*/;
            sprite.position.x = position.x/* + glyph.offset_x */;
            sprite.rotation = rotation;
            sprite.size.x = glyph.width;
            sprite.size.y = glyph.height;
            sprite.uv_position.x = glyph.x * 1.0f / 1024.f;
            sprite.uv_position.y = glyph.y * 1.0f / 1024.f;
            sprite.uv_size.x = glyph.width * 1.0f / 1024.f;
            sprite.uv_size.y = glyph.height * 1.0f / 1024.f;
            sprite.origin = vec2(-advance_x_sum - glyph.offset_x, -glyph.offset_y); // Модифицируем origin, а не позицию, чтобы было правильное вращение
            add_sprite();
            advance_x_sum += glyph.advance_x;
        }
    }
};
