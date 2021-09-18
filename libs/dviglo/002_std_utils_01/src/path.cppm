export module dviglo.path;

import dviglo.std_string_utils;

using namespace std;

// Пространство имён содержит функции для работы с путями
export namespace dv_path
{

inline void to_internal(string& path)
{
    replace(path, '\\', '/');
}

inline void to_native(string& path)
{
#ifdef _WIN32
    replace(path, '/', '\\');
#else
    replace(path, '\\', '/');
#endif
}

// Возвращает подстроку до последнего / (не включительно). Если / в пути нет, тогда возвращает "."
string get_directory_path(const string& file_path)
{
    size_t pos = file_path.find_last_of('/');

    if (pos == string::npos)
        return ".";

    return file_path.substr(0, pos);
}

} // export namespace dv_path

