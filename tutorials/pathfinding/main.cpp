// Модули приложения
//import app;

// Модули движка
#include <SDL.h> // SDL_main

// Стандартная библиотека
#include <clocale> // setlocale

int SDL_main(int argc, char* argv[])
{
    // Позволяет выводить UTF-8 в консоль в Windows. В Linux консоль в кодировке UTF-8 по умолчанию
    setlocale(LC_CTYPE, "en_US.UTF-8");

/*    App app;
    app.run(argc, argv);

    return app.exit_code();*/

    return 0;
}
