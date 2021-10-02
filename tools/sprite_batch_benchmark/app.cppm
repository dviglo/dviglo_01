export module app;

// Модули движка
import <glm/trigonometric.hpp>; // radians()
import <glm/vec2.hpp>;
import <SDL.h>;
import dviglo.sdl_utils; // dv_base_path()
import dviglo.scope_guard; // DvScopeGuard
import dviglo.sprite_batch; // DvSpriteBatch, DvSpriteFont
import dviglo.time; // DvTime

// Стандартная библиотека
import <string>;
import <vector>;
import <memory>;

using namespace std;
using namespace glm;

static const ivec2 WINDOW_SIZE{800, 600};

export struct App
{
    bool exiting = false; // Был ли запрос на завершение программы
    SDL_Window* window = nullptr;
    DvTime time;
    unique_ptr<DvSpriteBatch> sprite_batch;
    unique_ptr<DvSpriteFont> sprite_font_18;
    unique_ptr<DvSpriteFont> sprite_font_18_shadow;
    unique_ptr<DvTexture> texture_1;
    unique_ptr<DvTexture> texture_2;
    unique_ptr<DvTexture> texture_3;
    unique_ptr<DvTexture> texture_atlas;

    enum class Mode
    {
        use_atlas,
        not_use_atlas
    };
        
    Mode mode = Mode::use_atlas;
    bool use_alpha = true;

    void run()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            exit(1);
            return;
        }

        const DvScopeGuard sg_sdl_quit = [] { SDL_Quit(); };

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        window = SDL_CreateWindow(
            "Бенчмарк DvSpriteBatch",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            800, 600, SDL_WINDOW_OPENGL);

        if (!window)
        {
            exit(1);
            return;
        }

        const DvScopeGuard sg_destroy_window = [this]
        {
            SDL_DestroyWindow(window);
            window = nullptr;
        };

        SDL_GLContext gl_context = SDL_GL_CreateContext(window);

        if (!gl_context)
        {
            exit(1);
            return;
        }

        const DvScopeGuard sg_gl_delete_context = [gl_context] { SDL_GL_DeleteContext(gl_context); };

        SDL_GL_SetSwapInterval(0); // Отключаем вертикальную синхронизацию

        glewExperimental = GL_TRUE;
        GLenum glew_result = glewInit();

        if (glew_result != GLEW_OK)
        {
            exit(1);
            return;
        }

        sprite_font_18 = make_unique<DvSpriteFont>(dv_base_path() + "/sbb_data/fonts/ubuntu-r_18_simple.fnt");
        sprite_font_18_shadow = make_unique<DvSpriteFont>(dv_base_path() + "/sbb_data/fonts/ubuntu-r_18_simple_blurred.fnt");
        texture_1 = make_unique<DvTexture>(dv_base_path() + "/sbb_data/1.png");
        texture_2 = make_unique<DvTexture>(dv_base_path() + "/sbb_data/2.png");
        texture_3 = make_unique<DvTexture>(dv_base_path() + "/sbb_data/3.png");
        texture_atlas = make_unique<DvTexture>(dv_base_path() + "/sbb_data/atlas.png");
        sprite_batch = make_unique<DvSpriteBatch>();

        SDL_Event event;

        while (!exiting)
        {
            while (SDL_PollEvent(&event))
                handle_sdl_event(&event);

            time.new_frame();
            update();
            draw();
        }
    }

    void update()
    {
        //rotation += 180.0f * time().time_step_s();
    }

    void draw()
    {
        glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
        //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        SDL_GL_GetDrawableSize(window, &sprite_batch->screen_size.x, &sprite_batch->screen_size.y);

        sprite_batch->alpha_blending(use_alpha);

        sprite_batch->sprite.rotation = 0;
        sprite_batch->sprite.origin = vec2(0, 0);
        sprite_batch->sprite.size = {128, 128};

        const i32 NUM_ITERATIONS = 2000;
        const ivec2 MAX_SPRITE_POS{WINDOW_SIZE.x - 128, WINDOW_SIZE.y - 128};

        if (mode == Mode::use_atlas)
        {
            sprite_batch->texture(texture_atlas.get());
            sprite_batch->sprite.uv_size = vec2(1.f / 3, 1.f);

            for (i32 i = 0; i < NUM_ITERATIONS; i++)
            {
                sprite_batch->sprite.position = vec2(rand() % MAX_SPRITE_POS.x, rand() % MAX_SPRITE_POS.y);
                sprite_batch->sprite.uv_position = vec2(0, 0);
                sprite_batch->add_sprite();

                sprite_batch->sprite.position = vec2(rand() % MAX_SPRITE_POS.x, rand() % MAX_SPRITE_POS.y);
                sprite_batch->sprite.uv_position = vec2(1.f / 3, 0);
                sprite_batch->add_sprite();

                sprite_batch->sprite.position = vec2(rand() % MAX_SPRITE_POS.x, rand() % MAX_SPRITE_POS.y);
                sprite_batch->sprite.uv_position = vec2(2.f / 3, 0);
                sprite_batch->add_sprite();
            }
        }
        else if (mode == Mode::not_use_atlas)
        {
            sprite_batch->sprite.uv_position = vec2(0, 0);
            sprite_batch->sprite.uv_size = vec2(1, 1);

            for (i32 i = 0; i < NUM_ITERATIONS; i++)
            {
                sprite_batch->sprite.position = vec2(rand() % MAX_SPRITE_POS.x, rand() % MAX_SPRITE_POS.y);
                sprite_batch->texture(texture_1.get());
                sprite_batch->add_sprite();

                sprite_batch->sprite.position = vec2(rand() % MAX_SPRITE_POS.x, rand() % MAX_SPRITE_POS.y);
                sprite_batch->texture(texture_2.get());
                sprite_batch->add_sprite();

                sprite_batch->sprite.position = vec2(rand() % MAX_SPRITE_POS.x, rand() % MAX_SPRITE_POS.y);
                sprite_batch->texture(texture_3.get());
                sprite_batch->add_sprite();
            }
        }

        sprite_batch->alpha_blending(true);

        const vec2 TEXT_SHADOW_OFFSET{2, 0};

        string mode_str = (mode == Mode::use_atlas ? "Атлас используется"s : "Атлас не используется"s) + " (ПРОБЕЛ - изменить)"s;
        const vec2 MODE_STR_POS{0, WINDOW_SIZE.y - sprite_font_18->line_height() * 2};
        sprite_batch->draw_string(mode_str, sprite_font_18_shadow.get(), MODE_STR_POS + TEXT_SHADOW_OFFSET, 0xFF000000);
        sprite_batch->draw_string(mode_str, sprite_font_18.get(), MODE_STR_POS, 0xFFFFFFFF);

        string use_alpha_str = (use_alpha ? "Альфа используется"s : "Альфа не используется"s) + " (F1 - изменить)"s;
        const vec2 USE_ALPHA_STR_POS{ 0, WINDOW_SIZE.y - sprite_font_18->line_height() };
        sprite_batch->draw_string(use_alpha_str, sprite_font_18_shadow.get(), USE_ALPHA_STR_POS + TEXT_SHADOW_OFFSET, 0xFF000000);
        sprite_batch->draw_string(use_alpha_str, sprite_font_18.get(), USE_ALPHA_STR_POS, 0xFFFFFFFF);

        string fps_str = "ФПС: "s + to_string(time.fps());
        const vec2 FPS_STR_POS{0, 0};
        sprite_batch->draw_string(fps_str, sprite_font_18_shadow.get(), FPS_STR_POS + TEXT_SHADOW_OFFSET, 0xFF000000);
        sprite_batch->draw_string(fps_str, sprite_font_18.get(), FPS_STR_POS, 0xFFFFFFFF);

        string num_sprites_str = "Число спрайтов: "s + to_string(NUM_ITERATIONS * 3);
        const vec2 NUM_SPRITES_STR_POS = FPS_STR_POS + vec2(0, sprite_font_18->line_height());
        sprite_batch->draw_string(num_sprites_str, sprite_font_18_shadow.get(), NUM_SPRITES_STR_POS + TEXT_SHADOW_OFFSET, 0xFF000000);
        sprite_batch->draw_string(num_sprites_str, sprite_font_18.get(), NUM_SPRITES_STR_POS, 0xFFFFFFFF);

        sprite_batch->flush();

        SDL_GL_SwapWindow(window);
    }

    void handle_sdl_event(SDL_Event* e)
    {
        switch (e->type)
        {
        case SDL_QUIT:
            exiting = true;
            break;

        case SDL_KEYDOWN:
            switch (e->key.keysym.sym)
            {
            case SDLK_ESCAPE:
                exiting = true;
                break;

            case SDLK_SPACE:
                if (mode == Mode::use_atlas)
                    mode = Mode::not_use_atlas;
                else
                    mode = Mode::use_atlas;
                break;

            case SDLK_F1:
                use_alpha = !use_alpha;
                break;
            }
            break;
        }
    }
};
