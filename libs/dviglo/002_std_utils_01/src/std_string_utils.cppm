export module dviglo.std_string_utils;

// Модули движка
import dviglo.primitive_types; // u8

// Стандартная библиотека
export import <string>;
export import <vector>;
import <algorithm>; // replace

using namespace std;

export
{

inline bool contains(const string& str, char value)
{
    return str.find(value) != string::npos;
}

inline bool ends_with(const string& str, const string& value)
{
    if (value.size() > str.size())
        return false;

    return equal(value.rbegin(), value.rend(), str.rbegin());
}

inline void replace(string& str, char old_value, char new_value)
{
    replace(str.begin(), str.end(), old_value, new_value);
}

// Заменяет все подстроки в строке. Не изменяет исходную строку, а возвращает новую
string replace_copy(const string& str, const string& old_value, const string& new_value)
{
    string ret;

    size_t offset = 0;
    size_t pos = str.find(old_value); // Позиция old_value в исходной строке

    while (pos != string::npos)
    {
        ret.append(str, offset, pos - offset); // Копируем фрагмент до найденной подстроки
        ret += new_value;
        offset = pos + old_value.length(); // Смещение после найденной подстроки
        pos = str.find(old_value, offset);
    }

    ret += str.substr(offset); // Копируем остаток строки

    return ret;
}

// Обрезает конец строки, если там находятся любые символы из набора
void trim_end(string& str, const string& chars = " ")
{
    size_t new_length = str.length();

    while (new_length > 0)
    {
        char c = str.c_str()[new_length - 1];
        if (!contains(chars, c))
            break;

        --new_length;
    }

    if (new_length < str.length())
        str.resize(new_length);
}

// Обрезает начало и конец строки, если там находятся любые символы из набора.
// Не изменяет исходную строку, а возвращает новую
string trim_copy(const string& str, const string& chars = " ")
{
    size_t result_begin = 0;

    // В C++ принято, что end указывает на элемент после последнего
    size_t result_end = str.length();

    while (result_begin < result_end)
    {
        char c = str.c_str()[result_begin];
        if (!contains(chars, c))
            break;

        ++result_begin;
    }

    while (result_end > result_begin)
    {
        char c = str.c_str()[result_end - 1];
        if (!contains(chars, c))
            break;

        --result_end;
    }

    return str.substr(result_begin, result_end - result_begin);
}

vector<string> split(const string& str, const string& separator, bool keep_empty = true)
{
    vector<string> ret;

    if (separator.empty())
    {
        if (keep_empty || str.length())
            ret.push_back(str);

        return ret;
    }

    size_t offset = 0;
    size_t pos = str.find(separator); // Позиция разделителя в исходной строке

    while (pos != string::npos)
    {
        string substr = str.substr(offset, pos - offset); // Фрагмент до разделителя
        if (keep_empty || substr.length())
            ret.push_back(substr);

        offset = pos + separator.length(); // Смещение после разделителя
        pos = str.find(separator, offset);
    }

    // Остаток строки
    string last = str.substr(offset);
    if (keep_empty || last.length())
        ret.push_back(last);

    return ret;
}

// Извлекает очередной символ из UTF-8-строки (декодируя в UTF-32).
// Использование:
// size_t offset = 0;
// while (offset < str.length()) { u32 code_point = next_code_point(str, offset); }
u32 next_code_point(const string& str, size_t& offset)
{
    if (offset >= str.length())
        return 0;

    // Получаем байт из строки и после этого инкрементируем смещение
    u8 byte1 = str[offset++];

    // Если первый бит первого байта - ноль, значит последовательность состоит из единственного байта.
    // 0xxxxxxx (x - биты, в которой хранится кодовая позиция)
    if (byte1 <= 0b01111111u)
        return byte1;

    // Если мы дошли досюда, значит byte1 > 01111111, то есть byte1 >= 01111111 + 1, то есть byte1 >= 10000000

    // Первый байт не может начинаться с 10. Или offset указывает не на первый байт последовательности, или ошибка в буфере
    if (byte1 <= 0b10111111u) // Выше мы уже проверили, что byte1 >= 10000000
        goto Error;

    // Если первый байт начинается с 110, то последовательность имеет длину 2 байта.
    // 110xxxxx 10xxxxxx (x - биты, в которой хранится кодовая позиция)
    if (byte1 <= 0b11011111u) // Выше мы уже проверили, что byte1 >= 11000000
    {
        if (offset == str.length()) // Проверяем, что второй байт вообще есть (offset уже указывает на очередной байт)
            goto Error;

        // Получаем второй байт последовательности
        u8 byte2 = str[offset++];

        // Проверяем, что второй байт начинается с 10
        if ((byte2 & 0b11000000u) != 0b10000000u)
            goto Error;

        // Выкидываем из байтов служебную информацию.
        // 0b00111111u имеет тип u32, поэтому тут всё автоматически преобразуется к u32
        return (byte2 & 0b00111111u) | ((byte1 & 0b00011111u) << 6u);
    }

    // Если первый байт начинается с 1110, то последовательность имеет длину 3 байта.
    // 1110xxxx 10xxxxxx 10xxxxxx
    if (byte1 <= 0b11101111u) // Выше мы уже проверили, что byte1 >= 11100000
    {
        if (offset + 1 >= str.length())
            goto Error;

        u8 byte2 = str[offset++];
        u8 byte3 = str[offset++];

        // Проверяем, что второй и третий байты начинаются с 10
        if ((byte2 & 0b11000000u) != 0b10000000u || (byte3 & 0b11000000u) != 0b10000000u)
            goto Error;

        // Выкидываем из байтов служебную информацию
        return (byte3 & 0b00111111u) | ((byte2 & 0b00111111u) << 6u) | ((byte1 & 0b00001111u) << 12u);
    }

    // Если первый байт начинается с 11110, то последовательность имеет длину 4 байта.
    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    if (byte1 <= 0b11110111u) // Выше мы уже проверили, что byte1 >= 11110000
    {
        if (offset + 2 >= str.length())
            goto Error;

        u8 byte2 = str[offset++];
        u8 byte3 = str[offset++];
        u8 byte4 = str[offset++];

        if ((byte2 & 0b11000000u) != 0b10000000u || (byte3 & 0b11000000u) != 0b10000000u || (byte4 & 0b11000000u) != 0b10000000u)
            goto Error;

        return (byte4 & 0b00111111u) | ((byte3 & 0b00111111u) << 6u) | ((byte2 & 0b00111111u) << 12u) | ((byte1 & 0b00000111u) << 18u);
    }

Error:
    // При ошибке прерываем декодирование строки
    offset = str.length();

    // Необходимо что-то вернуть
    return '?';
}

} // export
