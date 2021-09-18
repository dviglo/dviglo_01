export module dviglo.file;

// Модули движка
import dviglo.log;

// Стандартная библиотека
import <format>;

using namespace std;

// Пространство имён содержит функции для работы с файлами
export namespace dv_file
{

// Читает содержимое всего файла в строку. В начале файла не должно быть BOM
string read_all_text(const string& path)
{
    // Используем самый быстрый способ: https://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html

    string ret;
    FILE* fp = fopen(path.c_str(), "rb");
    
    if (fp)
    {
        // Определяем размер файла и выделяем память
        fseek(fp, 0, SEEK_END);
        ret.resize(ftell(fp));
        rewind(fp);

        // Примечание: string::resize() заполняет память нулями, это лишняя операция.
        // Есть способ избежать этого: https://www.reddit.com/r/cpp/comments/6friuu/resize_stringvector_without_initialization/
        // Однако тут https://github.com/facebook/folly/blob/master/folly/memory/UninitializedMemoryHacks.h написано,
        // что memset() может сработать медленно из-за промаха кэша. Если убрать memset(), то промах просто возникнет в следующей операции.
        // Имеет смысл заморачиваться только если размер буфера превышает размер кэша

        fread(ret.data(), 1, ret.size(), fp);
    }
    else
    {
        LOG().write_error(format("dv_file::read_all_text(): !fp | path = \"{}\"", path));
    }

    return ret;
}

} // namespace dv_file
