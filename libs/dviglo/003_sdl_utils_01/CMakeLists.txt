# Название таргета
set(target_name dviglo_sdl_utils_01)

# Создаём список файлов
file(GLOB_RECURSE source_files src/*.cppm)

# Создаём статическую библиотеку
add_library(${target_name} STATIC ${source_files})

# Подключаем библиотеки к текущей библиотеке и к таргетам, использующим текущую библиотеку
target_link_libraries(${target_name} PUBLIC SDL2main SDL2-static dviglo_std_utils_01)

# Глубина зависимости: 003
# 000_sdl_01, 002_std_utils_01

# Заставляем VS отображать дерево каталогов
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${source_files})
