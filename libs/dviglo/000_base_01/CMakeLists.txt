# Название таргета
set(target_name dviglo_base_01)

# Создаём список файлов
file(GLOB_RECURSE source_files src/*.cppm)

# Создаём статическую библиотеку
add_library(${target_name} STATIC ${source_files})

# Заставляем VS отображать дерево каталогов
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${source_files})
