export module app;

// Модули движка
import <SDL.h>; // SDL_Window
//import <SDL_opengl.h>;
import <imgui.h>;
import <imgui_impl_sdl.h>;
import <imgui_impl_opengl3.h>;
import dviglo.localization; // DvLocalization
import dviglo.image; // DvImage
import dviglo.texture;
import dviglo.log; // LOG
import dviglo.scope_guard; // DvScopeGuard
import dviglo.sdl_utils; // dv_base_path, dv_pref_path

// Стандартная библиотека
import <string>;
import <stdio.h>;

// Модули приложения
import glyph_renderer;

using namespace std;

// Строку, которая меняется при смене языка, нельзя использовать как ID.
// Добавляем к строке постоянный ID
#define LOCALIZE(str, imgui_id) \
    (l10n[str] + "###" + imgui_id).c_str()

export struct App
{
    SDL_Window* window = nullptr;
    bool show_demo_window = true;
    DvLocalization l10n{&LOG()};

    // Настройки шрифта
    string font_path;

    // Был ли запрос на завершение программы
    bool exiting = false;

    DvImage* result = nullptr;
    DvTexture* texture = nullptr;

    void update()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        /*ImGui::Begin("Настройки", nullptr, 0);
        ImGui::Button("Ok", ImVec2(100, 0)); ImGui::SameLine();
        ImGui::Button("Ok");
        ImGui::End();*/
        show_app_settings();
        show_font_settings();

        ImGui::Render();
    }

    void draw()
    {
        const ImVec2& display_size = ImGui::GetIO().DisplaySize;
        glViewport(0, 0, (GLsizei)display_size.x, (GLsizei)display_size.y);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }
    
    void handle_sdl_event(SDL_Event& event)
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            exiting = true;
        else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            exiting = true;
    }
    
    void run()
    {
        LOG().open(dv_pref_path("dviglo", "bitmap font generator") + "/log.txt");
        l10n.load_json_file(dv_base_path() + "/bitmap_font_generator_data/strings.json");

        // Настройки шрифта
        font_path = dv_base_path() + "/bitmap_font_generator_data/fonts/Ubuntu-R.ttf";

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            LOG().write_error("App::run(): SDL_Init(...) < 0 | "s + SDL_GetError());
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
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
        window = SDL_CreateWindow("Bitmap Font Generator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);

        if (!window)
        {
            LOG().write_error("App::run(): !window_ | "s + SDL_GetError());
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
            LOG().write_error("App::begin_run(): !gl_context | "s + SDL_GetError());
            exit(1);
            return;
        }

        const DvScopeGuard sg_gl_delete_context = [gl_context] { SDL_GL_DeleteContext(gl_context); };

        SDL_GL_SetSwapInterval(1); // Включаем вертикальную синхронизацию

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = "bitmap_font_generator_imgui.ini";
        ImGui::StyleColorsDark();
        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL3_Init("#version 330");

        // ImGui использует свои указатели на функции OpenGL, а мои библиотеки используют GLEW, так что его тоже надо инициализировать
        glewExperimental = GL_TRUE;
        GLenum glew_result = glewInit();
        if (glew_result != GLEW_OK)
        {
            LOG().write_error("App::begin_run(): glew_result != GLEW_OK | "s + (const char*)glewGetErrorString(glew_result));
            exit(1);
            return;
        }

        ImFontConfig config;
        config.OversampleH = 2;
        config.OversampleV = 1;
        config.GlyphExtraSpacing.x = 1.0f;
        io.Fonts->AddFontFromFileTTF((dv_base_path() + "/bitmap_font_generator_data/fonts/Ubuntu-R.ttf").c_str(),
            18.0f, &config, io.Fonts->GetGlyphRangesCyrillic());

        bool show_demo_window = true;

        GlyphRenderer glyph_renderer;

        FT_Face face;
        FT_UInt glyph_index;

        FT_ULong char_code = 'U';

        FT_New_Face(glyph_renderer.library(), (dv_base_path() + "/bitmap_font_generator_data/fonts/Ubuntu-R.ttf").c_str(), 0, &face);
        FT_Set_Pixel_Sizes(face, 0, 24);
        //FT_ULong charCode = FT_Get_First_Char(face, &glyph_index);
        //FT_Load_Char(face, char_code, FT_LOAD_RENDER);
        glyph_index = FT_Get_Char_Index(face, char_code);

        RenderedGlyph rendered_glyph = glyph_renderer.render_simpe(face, glyph_index);
        DvImage res = rendered_glyph.to_image(0x90909090);


        /*result = new DvImage(1024, 1024, 4);
        result->clear(0xFF00FF00);
        result->pixel(0, 0, 0xFF0000FF);
        result->pixel(1, 1, 0xFFFFFFFF);

        texture = new DvTexture(*result);*/

        texture = new DvTexture(res);

        SDL_Event event;
        while (!exiting)
        {
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            while (SDL_PollEvent(&event))
                handle_sdl_event(event);

            update();
            draw();
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void show_font_settings() // Окно с настройками шрифта
    {
        // Положение и размер окна при первом запуске, когда еще не создан файл bitmap_font_generator_imgui.ini
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 20, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(260, 80), ImGuiCond_FirstUseEver);

        if (!ImGui::Begin(LOCALIZE("Настройки шрифта", "Font Settings"), nullptr, 0))
        {
            // Если окно свёрнуто, то не рендерим элементы в нём

            ImGui::End(); // Завершаем создание окна
            return;
        }

        const int BUF_SIZE = 1024;
        static char buf[1024];
        if (font_path.size() < BUF_SIZE)
            strcpy(buf, font_path.c_str());
        else
            buf[0] = '\0';
        ImGui::InputText(LOCALIZE("Исходный шрифт", "Source font"), buf, IM_ARRAYSIZE(buf));


        ImGui::Button(LOCALIZE("Генерировать!", "Generate!"));


        ImGui::End(); // Завершаем создание окна
    }
    
    void show_app_settings() // Окно с настройками приложения
    {
        // Положение и размер окна при первом запуске, когда еще не создан файл bitmap_font_generator_imgui.ini
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        const int DEFAULT_HEIGHT = 80;
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 20, main_viewport->Size.y - DEFAULT_HEIGHT - 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(280, DEFAULT_HEIGHT), ImGuiCond_FirstUseEver);

        if (!ImGui::Begin(LOCALIZE("Настройки приложения", "Application Settings"), nullptr, 0))
        {
            // Если окно свёрнуто, то не рендерим элементы в нём

            ImGui::End(); // Завершаем создание окна
            return;
        }

        // Выпадающий список
        if (ImGui::BeginCombo(LOCALIZE("Язык", "Language"), l10n["название языка"].c_str()))
        {
            // Если список открыт, то рендерим содержимое

            for (i32 language_index = 0; language_index < l10n.num_languages(); language_index++)
            {
                // Является ли пукт списка уже выбранным
                const bool is_selected = (language_index == l10n.current_language_index());

                if (ImGui::Selectable(l10n.get("название языка", language_index).c_str(), is_selected))
                {
                    // Если был клик по пункту списка, то меняем текущий язык
                    l10n.current_language_index(language_index);
                }
            }

            ImGui::EndCombo(); // Завершаем создание списка
        }

        // Временно показываем картинку
        ImGui::Image((ImTextureID)texture->gpu_object_name(), ImVec2(texture->width(), texture->height()));

        ImGui::End(); // Завершаем создание окна
    }

};
