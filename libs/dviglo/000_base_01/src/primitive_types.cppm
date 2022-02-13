/*
    Данный модуль определяет короткие имена для примитивных типов.

    Мотивация:
    1. В большинстве современных компиляторов размеры примитивных типов совпадают (за исключением long).
       Но стандарт C++ этого не требует: <https://en.cppreference.com/w/cpp/language/types>.
       Поэтому распространённой практикой является использование типов с фиксированным размером (например Uint32 в SDL).
    2. В стандарте C++11 были добавлены [типы с фиксированным размером](https://en.cppreference.com/w/cpp/types/integer),
       но их имена излишне длинные: имя uint32_t такое же длинное, как unsigned.
       Поэтому используем имена [как в Rust](https://doc.rust-lang.org/book/ch03-02-data-types.html).
    3. В Common Intermediate Language используются еще более короткие имена (i8 вместо i64),
       однако такой вариант встречается редко и может сбивать с толку.
*/

export module dviglo.primitive_types;

// Стандартная библиотека
import <cstdint>; // int8_t, uint32_t и т.п.

// Проверяем, что типы имеют ожидаемый размер
static_assert(sizeof(char) == 1);
static_assert(sizeof(int) == 4);
static_assert(sizeof(long) == 4 || sizeof(long) == 8); // (Win32, Win64, Unix32) || Unix64
static_assert(sizeof(long long) == 8);

export
{
    typedef int8_t   i8;
    typedef uint8_t  u8;
    typedef int16_t  i16;
    typedef uint16_t u16;
    typedef int32_t  i32;
    typedef uint32_t u32;
    typedef int64_t  i64;
    typedef uint64_t u64;
}
