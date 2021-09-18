export module dviglo.base;

// Стандартная библиотека
import <cstdint>; // int8_t, uint32_t и т.п.

// Проверяем, что типы имеют ожидаемый размер
static_assert(sizeof(char) == 1);
static_assert(sizeof(int) == 4);

// Используем свои короткие имена для встроенных типов, так как имя uint32_t такое же длинное, как unsigned.
// Используем имена как в Rust https://doc.rust-lang.org/book/ch03-02-data-types.html
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
