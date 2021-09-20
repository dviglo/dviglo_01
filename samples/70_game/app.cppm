export module app;

// Модули приложения
import consts;
import config;
import sprite_batch;

// Модули движка
import <glm/trigonometric.hpp>; // radians()
import <SDL.h>;
import <SDL_mixer.h>;
import dviglo.localization; // DvLocalization
import dviglo.scope_guard; // DvScopeGuard
import dviglo.sdl_utils; // dv_base_path(), dv_pref_path()
import dviglo.time; // DvTime

// Стандартная библиотека
import <string>;
import <vector>;

using namespace std;
using namespace glm;

// Чтобы гарантировать правильный порядок создания и уничтожения объектов, не следует использовать глобальные переменные.
// Поместите глобальные данные в этот класс. Вы можете контролировать порядок конструкторов и деструкторов членов:
// https://stackoverflow.com/questions/2254263/order-of-member-constructor-and-destructor-calls
export class App
{
public:
    static App& get()
    {
        // В C++11 синглтон Майерса является потокобезопасным: https://stackoverflow.com/a/1661564
        static App instance;
        return instance;
    }

private:
    // Аргументы командной строки. Инициализируется в run()
    vector<string> args_;

    // Значение, которое вернёт main()
    i32 exit_code_ = 0;

    // Был ли запрос на завершение программы
    bool exiting_ = false;

    DvLocalization l10n_{&LOG()};

    SDL_Window* window_ = nullptr;

    DvTime time_;

    float rotation = 0.f;

    // Конструктор спрятан, экземляр создаётся в get()
    App()
    {
    }

    ~App()
    {
    }

    // Вызывается в начале run()
    void begin_run()
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        {
            LOG().write_error("App::begin_run(): SDL_Init(...) < 0 | "s + SDL_GetError());
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
        //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

        //u32 flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
        //if (config::window_maximized)
        //    flags |= SDL_WINDOW_MAXIMIZED;
        u32 flags = SDL_WINDOW_OPENGL;
        window_ = SDL_CreateWindow(
            l10n_["Игра"].c_str(),
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            config::window_width, config::window_height, flags);
        if (!window_)
        {
            LOG().write_error("App::begin_run(): !window_ | "s + SDL_GetError());
            exit(1);
            return;
        }

        const DvScopeGuard sg_destroy_window = [this]
        {
            SDL_DestroyWindow(window_);
            window_ = nullptr;
        };

        SDL_GLContext gl_context = SDL_GL_CreateContext(window_);
        if (!gl_context)
        {
            LOG().write_error("App::begin_run(): !gl_context | "s + SDL_GetError());
            exit(1);
            return;
        }

        const DvScopeGuard sg_gl_delete_context = [gl_context] { SDL_GL_DeleteContext(gl_context); };

        SDL_GL_SetSwapInterval(0); // Отключаем вертикальную синхронизацию
        //SDL_GL_SetSwapInterval(-1);

        glewExperimental = GL_TRUE;
        GLenum glew_result = glewInit();
        if (glew_result != GLEW_OK)
        {
            LOG().write_error("App::begin_run(): glew_result != GLEW_OK | "s + (const char*)glewGetErrorString(glew_result));
            exit(1);
            return;
        }

        LOG().write_info("GL_VENDOR: "s + (const char*)glGetString(GL_VENDOR));
        LOG().write_info("GL_RENDERER: "s + (const char*)glGetString(GL_RENDERER));
        LOG().write_info("GL_VERSION: "s + (const char*)glGetString(GL_VERSION));

        sprite_font = make_unique<DvSpriteFont>(dv_base_path() + "/70_data/fonts/ubuntu-b-30-outlined.fnt");

        //texture = make_unique<DvTexture>(base_path() + "/70_data/awesomeface.png");
        //texture = make_unique<DvTexture>(base_path() + "/70_data/fonts/ubuntu-b-30-outlined_0.png");
        texture = make_unique<DvTexture>(dv_base_path() + "/70_data/1.png");

        sprite_batch = make_unique<DvSpriteBatch>();


        // Прячем задние стороны спрайтов, чтобы быть уверенным, что все спрайты отрисовываются правильно против часовой стрелки
        glEnable(GL_CULL_FACE);

        // Открываем аудио-устройство
        if (Mix_OpenAudio(MIX_DEFAULT_FORMAT, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
        {
            LOG().write_error("App::begin_run(): Mix_OpenAudio(...) < 0 | "s + Mix_GetError());
            exit(1);
        }

        const DvScopeGuard sg_close_audio = [] { Mix_CloseAudio(); };

        Mix_Music* music = Mix_LoadMUS((dv_base_path() + "/70_data/CantinaBand60.wav").c_str());
        if (!music)
        {
            LOG().write_error("App::begin_run(): !music | "s + Mix_GetError());
            exit(1);
        };

        const DvScopeGuard sg_free_music = [music] { Mix_FreeMusic(music); };

        Mix_PlayMusic(music, -1);

        SDL_Event event;

        while (!exiting())
        {
            while (SDL_PollEvent(&event))
                handle_sdl_event(&event);

            time_.new_frame();
            update();
            draw();

            // ФПС падает с 5000 до 500, но если игра и так не летает, то +1 мс задержки будет долей процента.
            // Таким орбазом ФПС просто никогда не будет запредельным
            //SDL_Delay(1);
        }
    }

    // Вызывается в конце run()
    void end_run()
    {

    }

    void update()
    {
        rotation += 180.0f * time().time_step_s();
    }

    void draw()
    {
        glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);



        SDL_GL_GetDrawableSize(window_, &sprite_batch->screen_size.x, &sprite_batch->screen_size.y);

        sprite_batch->alpha_blending(true);


        //    rotation = 0;

        sprite_batch->texture(texture.get());
        sprite_batch->sprite.color_ul = 0xFF00FF00;
        sprite_batch->sprite.color_ur = 0xFFFF0000;
        sprite_batch->sprite.color_dr = 0xFF0000FF;
        sprite_batch->sprite.color_dl = 0xFFFF00FF;
        sprite_batch->sprite.uv_position = vec2(0, 0);
        sprite_batch->sprite.uv_size = vec2(1, 1);
        sprite_batch->sprite.rotation = radians(rotation);
        sprite_batch->sprite.origin = vec2(0, 0);
        sprite_batch->sprite.position = vec2(400, 300);
        sprite_batch->sprite.size = { 400, 300 };
        sprite_batch->add_sprite();

        string fps_str = "ФПС: "s + to_string(time().fps());
        sprite_batch->draw_string(fps_str, sprite_font.get(), vec2(400, 300), 0xFF00FF00, radians(rotation));

        sprite_batch->draw_string(fps_str, sprite_font.get(), vec2(0, 0), 0xFF00FF00);

        sprite_batch->flush();

        SDL_GL_SwapWindow(window_);
    }

    void handle_sdl_event(SDL_Event* e)
    {
        switch (e->type)
        {
        case SDL_QUIT:
            exit();
            break;

        case SDL_WINDOWEVENT:
            {
                if (e->window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    // Не запоминаем размеры окна, если оно развернуто на весь экран.
                    u32 flags = SDL_GetWindowFlags(window_);
                    if (!(flags & SDL_WINDOW_MAXIMIZED))
                    {
                        config::window_width = e->window.data1;
                        config::window_height = e->window.data2;
                    }
                }
                else if (e->window.event == SDL_WINDOWEVENT_MAXIMIZED)
                {
                    config::window_maximized = true;
                }
                else if (e->window.event == SDL_WINDOWEVENT_RESTORED)
                {
                    config::window_maximized = false;
                }
            }
            break;
        }
    }

public:
    inline const DvTime& time() const
    {
        return time_;
    }

    unique_ptr<DvTexture> texture;

    // Аргументы командной строки. Инициализируется в run()
    inline const vector<string>& args()
    {
        return args_;
    }

    // Значение, которое вернёт main()
    inline i32 exit_code() const
    {
        return exit_code_;
    }

    // Был ли запрос на завершение программы
    inline bool exiting() const
    {
        return exiting_;
    }

    // Запросить завершение программы
    void exit(i32 exit_code = 0)
    {
        exit_code_ = exit_code;
        exiting_ = true;
    }

    inline DvLocalization& l10n()
    {
        return l10n_;
    }

    // Запуск приложения
    void run(i32 argc, char* argv[])
    {
#ifndef NDEBUG
        // Убеждаемся, что функция run() не была вызвана повторно
        static bool called = false;
        assert(!called);
        called = true;
#endif

        // Копируем аргументы в вектор строк
        args_.reserve(argc);
        for (i32 i = 0; i < argc; ++i)
            args_.push_back(argv[i]);

        LOG().open(dv_pref_path(ORG_NAME, APP_NAME) + "/log.txt");
        l10n().load_json_file(dv_base_path() + "/70_data/strings.json");
        config::load();

        begin_run();
        end_run();

        config::save();
        LOG().close();
    }

    // ================= ВРЕМЕННО

    unique_ptr<DvSpriteBatch> sprite_batch;

    unique_ptr<DvSpriteFont> sprite_font;
};
