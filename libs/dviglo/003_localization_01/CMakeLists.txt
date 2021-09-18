# Название таргета
set(target_name dviglo_localization_01)

# Создаём список файлов
file(GLOB_RECURSE source_files src/*.cppm)

# Создаём статическую библиотеку
add_library(${target_name} STATIC ${source_files})

# Подключаем библиотеки к текущей библиотеке
target_link_libraries(${target_name} PRIVATE rapidjson_01 dviglo_std_utils_01)

# Глубина зависимости: 003
# 000_rapidjson_01, 002_std_utils_01

# Заставляем VS отображать дерево каталогов
source_group(TREE  ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${source_files})
