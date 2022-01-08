module;

// Не импортируется нормально, инклудим
#include <rapidjson/document.h>

export module dviglo.localization;

// Модули движка
export import dviglo.base; // i32
export import dviglo.log; // DvLog
import <rapidjson/document.h>;
import <rapidjson/error/en.h>;
import dviglo.file; // dv_file
import dviglo.std_vector_utils; // contains

// Стандартная библиотека
import <unordered_map>;
export import <vector>;
import <format>;

using namespace rapidjson;
using namespace std;

export class DvLocalization
{
private:
    // Идентификаторы языков
    vector<string> language_ids_;

    // Индекс текущего языка. Если меньше нуля, значит еще нет ни одного языка
    i32 current_language_index_ = -1;

    // Хранилище строк: <идентификатор_языка <идентификатор_строки, строка>>
    unordered_map<string, unordered_map<string, string>> strings_;

public:
    DvLog* log = nullptr;

    // Не писать предупреждения в лог (только ошибки)
    bool silent = false;

    inline const vector<string>& language_ids() const
    {
        return language_ids_;
    }

    inline i32 num_languages() const
    {
        return (i32)language_ids_.size();
    }

    // Индекс текущего языка
    inline i32 current_language_index() const
    {
        return current_language_index_;
    }

    // Индекс текущего языка
    void current_language_index(i32 index)
    {
        if (num_languages() == 0)
        {
            if (log)
                log->write_error("DvLocalization::current_language_index(i32): num_languages() == 0");

            return;
        }

        if (index < 0 || index >= num_languages())
        {
            if (log)
                log->write_error("DvLocalization::current_language_index(i32): index < 0 || index >= num_languages()");

            return;
        }

        current_language_index_ = index;
    }

    // Идентификатор текущего языка
    string current_language_id() const
    {
        if (current_language_index_ == -1)
        {
            if (log)
                log->write_error("DvLocalization::current_language_id(): current_language_index_ == -1");

            return string();
        }

        return language_ids_[current_language_index_];
    }

    // Добавляет строку в хранилище
    void add_string(const string& language, const string& id, const string& value)
    {
        strings_[language][id] = value;

        // Если языка еще нет в списке, добавляем его
        if (!contains(language_ids_, language))
            language_ids_.push_back(language);

        if (current_language_index_ == -1)
            current_language_index_ = 0;
    }

    // Возвращает строку на текущем языке. Если перевод не найден, то возвращает id
    string get(const string& id) const
    {
        if (id.empty())
            return string();

        if (num_languages() == 0)
        {
            if (log && !silent)
                log->write_warning("DvLocalization::get(): num_languages() == 0");

            return id;
        }

        const unordered_map<string, string>& m = strings_.at(current_language_id());
        unordered_map<string, string>::const_iterator i = m.find(id);
        if (i == m.end()) // Нет строки с таким идентификатором
        {
            if (log && !silent)
                log->write_warning(format("DvLocalization::get(): i == m.end() | id = {} | current_language_id() = {}", id, current_language_id()));

            return id;
        }

        return i->second;
    }

    // Возвращает строку на определённом языке. Если перевод не найден, то возвращает id
    string get(const string& id, i32 language_index) const
    {
        if (id.empty())
            return string();

        if (num_languages() == 0)
        {
            if (log && !silent)
                log->write_warning("DvLocalization::get(): num_languages() == 0");

            return id;
        }

        if (language_index < 0 || language_index >= num_languages())
        {
            if (log && !silent)
                log->write_warning("DvLocalization::get(): language_index < 0 || language_index >= num_languages()");

            return id;
        }

        const unordered_map<string, string>& m = strings_.at(language_ids_[language_index]);
        unordered_map<string, string>::const_iterator i = m.find(id);
        if (i == m.end()) // Нет строки с таким идентификатором
        {
            if (log && !silent)
                log->write_warning(format("DvLocalization::get(): i == m.end() | id = {} | language_index = {}", id, language_index));

            return id;
        }

        return i->second;
    }

    // Вызывает get()
    inline string operator[](const string& id) const
    {
        return get(id);
    }

    // Загружает словарь из json-файла
    void load_json_file(const string& path)
    {
        string content = dv_file::read_all_text(path);
        Document document;
        const i32 flags = ParseFlag::kParseCommentsFlag | ParseFlag::kParseTrailingCommasFlag;
        ParseResult parse_result = document.Parse<flags>(content.c_str());

        if (!parse_result)
        {
            LOG().write_error(
                format("DvLocalization::load_json_file(): !parse_result | path = {} | offset = {} | {}",
                    path, parse_result.Offset(), GetParseError_En(parse_result.Code()))
            );

            return;
        }

        if (!document.IsObject())
        {
            LOG().write_error("DvLocalization::load_json_file(): !parse_result | path = " + path);
            return;
        }

        for (auto& member : document.GetObject())
        {
            if (!member.value.IsObject())
            {
                LOG().write_error("DvLocalization::load_json_file(): !member.value.IsObject() | path = " + path);
                return;
            }

            string id(member.name.GetString());

            for (auto& translation : member.value.GetObj())
            {
                if (!translation.value.IsString())
                {
                    LOG().write_error("DvLocalization::load_json_file(): !translation.value.IsString() | path = " + path);
                    return;
                }

                string language(translation.name.GetString());
                string value(translation.value.GetString());
                add_string(language, id, value);
            }
        }
    }

    inline DvLocalization(DvLog* log = nullptr)
        : log(log)
    {
    }
};
