module;

#define RAPIDJSON_HAS_STDSTRING 1
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#include <fstream>

export module config;

// Модули приложения
import consts;

// Модули движка
import <string>;
import dviglo.file;  // read_all_text()
import dviglo.log; // DvLog
import dviglo.sdl_utils; // dv_pref_path()

// Стандартная библиотека

using namespace std;

static const string WINDOW_WIDTH_STR = "window_width";
static const string WINDOW_HEIGHT_STR = "window_height";
static const string WINDOW_MAXIMIZED_STR = "window_maximized";

export namespace config
{
    // Значения по умолчанию задаются тут.
    i32 window_width = 800;
    i32 window_height = 600;
    bool window_maximized = true;

    void load()
    {
        // Должно вызываться после иниц SDL?
        static const string CONFING_FILE_PATH = dv_pref_path(ORG_NAME, APP_NAME) + "/config.json";

        ifstream stream(CONFING_FILE_PATH);
        if (!stream.is_open())
        {
            LOG().write_warning("Could not open file " + CONFING_FILE_PATH);
            return;
        }

        LOG().write_info("Loading config " + CONFING_FILE_PATH);

        string text{istreambuf_iterator<char>(stream), istreambuf_iterator<char>()};
        rapidjson::Document document;
        document.Parse(text);

        if (document.HasMember(WINDOW_WIDTH_STR) && document[WINDOW_WIDTH_STR].IsInt())
            window_width = document[WINDOW_WIDTH_STR].GetInt();

        if (document.HasMember(WINDOW_HEIGHT_STR) && document[WINDOW_HEIGHT_STR].IsInt())
            window_height = document[WINDOW_HEIGHT_STR].GetInt();

        if (document.HasMember(WINDOW_MAXIMIZED_STR) && document[WINDOW_MAXIMIZED_STR].IsBool())
            window_maximized = document[WINDOW_MAXIMIZED_STR].GetBool();

        stream.close();
    }
    
    void save()
    {
        static const string CONFING_FILE_PATH = dv_pref_path(ORG_NAME, APP_NAME) + "/config.json";

        ofstream stream(CONFING_FILE_PATH);
        if (!stream.is_open())
        {
            LOG().write_error("Could not create file " + CONFING_FILE_PATH);
            return;
        }

        LOG().write_info("Saving config " + CONFING_FILE_PATH);

        rapidjson::Document document;
        document.SetObject();
        rapidjson::MemoryPoolAllocator<>& allocator = document.GetAllocator();

        document.AddMember(rapidjson::StringRef(WINDOW_WIDTH_STR), window_width, allocator);
        document.AddMember(rapidjson::StringRef(WINDOW_HEIGHT_STR), window_height, allocator);
        document.AddMember(rapidjson::StringRef(WINDOW_MAXIMIZED_STR), window_maximized, allocator);

        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        writer.SetIndent(' ', 4);
        document.Accept(writer);
        stream << buffer.GetString();
        stream.close();
    }
}
