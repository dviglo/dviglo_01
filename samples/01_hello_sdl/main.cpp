// Пример показывает окно 3 секунды
// https://wiki.libsdl.org/SDL_CreateWindow

// Модули движка
import dviglo.log; // LOG
import dviglo.scope_guard; // DvScopeGuard
import dviglo.sdl_utils; // dv_pref_path
#include <SDL.h>

using namespace std;

int SDL_main(int argc, char* argv[])
{
    // В Windows 10 путь к логу c:\Users\[ИМЯ ПОЛЬЗОВАТЕЛЯ]\AppData\Roaming\dviglo\01_hello_sdl\log.txt
    LOG().open(dv_pref_path("dviglo", "01_hello_sdl") + "/log.txt");
    LOG().write_info("Старт");

    // Лог будет закрыт, когда объект sg_log_close выйдет из области видимости
    // (лямбда-функция вызывается в деструкторе этого объекта).
    // Так как этот DvScopeGuard создан первым, то его деструктор будет вызван последним (такова особенность C++)
    const DvScopeGuard sg_log_close = [] { LOG().close(); };

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        LOG().write_error("SDL_Init(SDL_INIT_VIDEO) < 0 | "s + SDL_GetError());

        // Тут будет автоматически вызвана функция LOG().close();
        return 1;
    }

    // Этот DvScopeGuard создан вторым, поэтому эта лямда-функция будет вызвана перед закрытием лога
    const DvScopeGuard sg_sdl_quit = [] { SDL_Quit(); };

    SDL_Window* window = SDL_CreateWindow(
        "Окно закроется через 3 секунды",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
        0
    );

    if (!window)
    {
        LOG().write_error("!window | "s + SDL_GetError());

        // Тут будут автоматически вызваны:
        // SDL_Quit();
        // LOG().close();
        return 1;
    }

    const DvScopeGuard sg_destroy_window = [window] { SDL_DestroyWindow(window); };

    SDL_Surface* screen_surface = SDL_GetWindowSurface(window);
    SDL_FillRect(screen_surface, nullptr, SDL_MapRGB(screen_surface->format, 0x10, 0x90, 0x90));
    SDL_UpdateWindowSurface(window);
    SDL_Delay(3000);

    LOG().write_info("Финиш");
    
    // Тут будут автоматически вызваны:
    // SDL_DestroyWindow();
    // SDL_Quit();
    // LOG().close();
    return 0;
}
