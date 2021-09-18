# Название таргета
set(target_name dviglo_log_01)

# Создаём список файлов
file(GLOB_RECURSE source_files src/*.cppm)

# Создаём статическую библиотеку
add_library(${target_name} STATIC ${source_files})

# Подключаем библиотеки к текущей библиотеке и к таргетам, использующим текущую библиотеку
target_link_libraries(${target_name} PUBLIC dviglo_base_01)

# Глубина зависимости: 001
# 000_base_01

# Заставляем VS отображать дерево каталогов
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${source_files})
