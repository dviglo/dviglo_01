export module dviglo.sdl_utils;

// Модули движка
import <SDL.h>; // SDL_GetPrefPath(), SDL_GetBasePath()
import dviglo.log; // dvLog
import dviglo.path; // dv_path

// Стандартная библиотека
import <cassert>; // assert()
import <cstdlib>; // exit()
import <format>;
import <iostream>; // cerr

using namespace std;

// Папка для пользовательских данных. В конце пути нет /.
// При неудаче завершает приложение
export string dv_pref_path(const string& org_name, const string& app_name)
{
    char* pref_path = SDL_GetPrefPath(org_name.c_str(), app_name.c_str());

    if (!pref_path)
    {
        // Обычно этот путь используется для лога, так что мы даже в лог ничего вывести не можем
        cerr << "!pref_path";
        exit(1);
    }
    else
    {
        string ret(pref_path);
        SDL_free(pref_path);
        dv_path::to_internal(ret);

        // Убираем / в конце пути
        assert(!ret.empty() && ret.back() == '/');
        ret.pop_back();

        return ret;
    }
}

// Папка с экзешником. В конце пути нет /
export string dv_base_path()
{
    char* base_path = SDL_GetBasePath();

    if (!base_path)
    {
        LOG().write_error(format("base_path(): !base_path | {}", SDL_GetError()));
        return "./";
    }
    else
    {
        string ret(base_path);
        SDL_free(base_path);
        dv_path::to_internal(ret);

        // Убираем / в конце пути
        assert(!ret.empty() && ret.back() == '/');
        ret.pop_back();

        return ret;
    }
}
