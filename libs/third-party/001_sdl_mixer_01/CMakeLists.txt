# Название таргета
set(target_name sdl_mixer_01)

# Создаём список файлов.
# Не используем рекурсию, так как подпапки в папке repo/src/codecs не нужны
file(GLOB source_files repo/src/*.c repo/src/*.h repo/src/codecs/*.c repo/src/codecs/*.h repo/include/*.h)

# Создаём статическую библиотеку
add_library(${target_name} STATIC ${source_files})

# Дефайны, используемые при компиляции текущей библиотеки
target_compile_definitions(${target_name} PRIVATE MUSIC_WAV)

# Делаем заголовочные файлы доступными текущей библиотеке
target_include_directories(${target_name} PRIVATE repo/src repo/src/codecs)

# Делаем заголовочные файлы доступными библиотеке и таргетам, которые используют текущую библиотеку
target_include_directories(${target_name} PUBLIC repo/include)

# Подключаем библиотеки к текущей библиотеке и к таргетам, использующим текущую библиотеку
target_link_libraries(${target_name} PUBLIC SDL2main SDL2-static)

# Глубина зависимости: 001
# 000_sdl_01

# Заставляем VS отображать дерево каталогов
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/repo FILES ${source_files})
