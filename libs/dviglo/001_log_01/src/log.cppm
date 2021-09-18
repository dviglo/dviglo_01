module;

// Не импортируется нормально, инклудим
#include <ctime> // time_t, ctime

export module dviglo.log;

// Модули движка
export import dviglo.base; // i32

// Стандартная библиотека
import <cassert>; // assert
import <fstream>; // ofstream
import <iostream>; // cout
export import <string>;

using namespace std;

export class dvLog
{
public:
    enum class MessageType : i32
    {
        debug = 0, // 0
        info,      // 1
        warning,   // 2
        error,     // 3
        none       // 4
    };

private:
    ofstream stream_;

public:
    inline void open(const string& path)
    {
        stream_.open(path);
        write_info("Log file open");
    }

    inline bool is_open() const
    {
        return stream_.is_open();
    }

    inline void close()
    {
        write_info("Log file closed");
        stream_.close();
    }

public:
    // Уровень сообщения, который будет выводиться в лог
    MessageType level = MessageType::debug;

    // Уровень сообщения, который будет выводится в консоль
    MessageType echo_level = MessageType::error;

public:
    void write(MessageType message_type, const string& message);

    inline void write_debug(const string& message)
    {
        write(MessageType::debug, message);
    }

    inline void write_info(const string& message)
    {
        write(MessageType::info, message);
    }

    inline void write_warning(const string& message)
    {
        write(MessageType::warning, message);
    }

    inline void write_error(const string& message)
    {
        write(MessageType::error, message);
    }

public:
    dvLog() = default;
    
    ~dvLog()
    {
        if (is_open())
        {
            // Лучше закрывать лог вручную, уничтожив перед этим все объекты в правильном порядке
            write_warning("Log file closed in destructor");
            stream_.close();
        }
    }

public:
    // Глобальный лог для удобства. Также можно создавать специализированные логи через конструктор
    static dvLog& get()
    {
        // В C++11 синглтон Майерса является потокобезопасным: https://stackoverflow.com/a/1661564
        static dvLog instance;
        return instance;
    }
};

export inline dvLog& LOG()
{
    return dvLog::get();
}

static inline string get_timestamp()
{
    time_t now = time(nullptr);
    string ret = ctime(&now);

    // Убираем \n в конце строки
    assert(!ret.empty() && ret.back() == '\n');
    ret.pop_back();

    return ret;
}

static inline const string& to_string(dvLog::MessageType message_type)
{
    assert((i32)message_type >= 0 && (i32)message_type <= 3);

    static string strings[] {
        "[DEBUG]",   // 0
        "[INFO]",    // 1
        "[WARNING]", // 2
        "[ERROR]"    // 3
    };

    return strings[(i32)message_type];
}

void dvLog::write(MessageType message_type, const string& message)
{
    if (message_type == MessageType::none)
        return;

    string timestamp = get_timestamp();
    const string& message_type_str = to_string(message_type);

    if (message_type >= level)
        stream_ << timestamp << " " << message_type_str << " " << message << endl;

    if (message_type >= echo_level)
        cout << timestamp << " " << message_type_str << " " << message << endl;
}
