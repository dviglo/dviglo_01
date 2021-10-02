//#include <imgui.h>
//#include <imgui_impl_sdl.h>
//#include <imgui_impl_opengl3.h>
#include <SDL_opengl.h>

export module app;

// Модули движка
import <SDL.h>;
import dviglo.log;
import dviglo.scope_guard; // DvScopeGuard
import <imgui.h>;
import <imgui_impl_sdl.h>;
import <imgui_impl_opengl3.h>;
import dviglo.sdl_utils; // dv_base_path(), dv_pref_path()
//import <SDL_opengl.h>;
import <stdio.h>;

// Стандартная библиотека
import <string>;

using namespace std;

export struct App
{
    void run()
    {
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
        SDL_Window* window = SDL_CreateWindow("Bitmap Font Generator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);

        if (!window)
        {
            LOG().write_error("App::run(): !window_ | "s + SDL_GetError());
            exit(1);
            return;
        }

        const DvScopeGuard sg_destroy_window = [&window]
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
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL3_Init("#version 330 core");

        ImFontConfig config;
        config.OversampleH = 2;
        config.OversampleV = 1;
        config.GlyphExtraSpacing.x = 1.0f;
        io.Fonts->AddFontFromFileTTF((dv_base_path() + "/bitmap_font_generator_data/fonts/Ubuntu-R.ttf").c_str(),
            18.0f, &config, io.Fonts->GetGlyphRangesCyrillic());

        bool show_demo_window = true;

        bool done = false;
        while (!done)
        {
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                ImGui_ImplSDL2_ProcessEvent(&event);
                if (event.type == SDL_QUIT)
                    done = true;
                else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                    done = true;
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            if (show_demo_window)
                ImGui::ShowDemoWindow(&show_demo_window);

            ImGui::Begin("Название окна", nullptr, 0);
            ImGui::End();

            ImGui::Render();

            glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            SDL_GL_SwapWindow(window);
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }
};
