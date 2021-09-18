# Название таргета
set(target_name 20_imgui)

# Создаём список файлов
set(examples_dir ${CMAKE_SOURCE_DIR}/libs/third-party/000_imgui_01/repo/examples)
file(GLOB_RECURSE source_files ${examples_dir}/example_sdl_opengl3/*.cpp)

# Создаём приложение
dviglo_add_executable(${target_name} ${source_files})

# Подключаем библиотеки
target_link_libraries(${target_name} PRIVATE imgui_sdl_gl3_01)

# Куда будет помещён скомпилированный исполняемый файл
dviglo_set_bin_dir(${target_name} ${CMAKE_BINARY_DIR}/bin)

# Заставляем VS отображать дерево каталогов
source_group(TREE ${examples_dir}/example_sdl_opengl3 FILES ${source_files})

