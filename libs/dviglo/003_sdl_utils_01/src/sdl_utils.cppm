export module dviglo.sdl_utils;

// ������ ������
import <SDL.h>; // SDL_GetPrefPath(), SDL_GetBasePath()
import dviglo.log; // dvLog
import dviglo.path; // dv_path

// ����������� ����������
import <cassert>; // assert()
import <cstdlib>; // exit()
import <format>;
import <iostream>; // cerr

using namespace std;

// ����� ��� ���������������� ������. � ����� ���� ��� /.
// ��� ������� ��������� ����������
export string dv_pref_path(const string& org_name, const string& app_name)
{
    char* pref_path = SDL_GetPrefPath(org_name.c_str(), app_name.c_str());

    if (!pref_path)
    {
        // ������ ���� ���� ������������ ��� ����, ��� ��� �� ���� � ��� ������ ������� �� �����
        cerr << "!pref_path";
        exit(1);
    }
    else
    {
        string ret(pref_path);
        SDL_free(pref_path);
        dv_path::to_internal(ret);

        // ������� / � ����� ����
        assert(!ret.empty() && ret.back() == '/');
        ret.pop_back();

        return ret;
    }
}

// ����� � ����������. � ����� ���� ��� /
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

        // ������� / � ����� ����
        assert(!ret.empty() && ret.back() == '/');
        ret.pop_back();

        return ret;
    }
}
