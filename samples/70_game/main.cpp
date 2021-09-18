// Модули приложения
import app;

// Модули движка
#include <SDL.h> // SDL_main

// Стандартная библиотека
#include <clocale> // setlocale

// Предпочитаем дискретную видеокарту, а не встроенную
#ifdef _WIN32
extern "C"
{
    // http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
    __declspec(dllexport) unsigned long NvOptimusEnablement = 1;

    // http://developer.amd.com/community/blog/2015/10/02/amd-enduro-system-for-developers/
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

int SDL_main(int argc, char* argv[])
{
    // Позволяет выводить UTF-8 в консоль в Windows. В Linux консоль в кодировке UTF-8 по умолчанию
    setlocale(LC_CTYPE, "en_US.UTF-8");

    // Создание экземпляра класса происходит при первом вызове get()
    App& app = App::get();

    app.run(argc, argv);

    return app.exit_code();
}
