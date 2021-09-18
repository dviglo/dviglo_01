# Название таргета
set(target_name 01_hello_sdl)

# Создаём список файлов
file(GLOB_RECURSE source_files *.cpp)

# Создаём приложение
dviglo_add_executable(${target_name} ${source_files})

# Подключаем библиотеки
target_link_libraries(${target_name} PRIVATE dviglo_sdl_utils_01)

# Куда будет помещён скомпилированный исполняемый файл
dviglo_set_bin_dir(${target_name} ${CMAKE_BINARY_DIR}/bin)

# Заставляем VS отображать дерево каталогов
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_files})
