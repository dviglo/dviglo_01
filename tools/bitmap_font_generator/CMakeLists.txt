# Название таргета
set(target_name bitmap_font_generator)

# Создаём список файлов
file(GLOB_RECURSE source_files *.cpp *.h *.cppm)

# Создаём приложение
dviglo_add_executable(${target_name} ${source_files})

# Подключаем библиотеки
target_link_libraries(${target_name} PRIVATE dviglo_sdl_utils_01 dviglo_sprite_batch_01 dviglo_localization_01 sdl_mixer_01 rapidjson_01 imgui_sdl_gl3_01)

# Куда будет помещён скомпилированный исполняемый файл
set(bin_dir ${CMAKE_BINARY_DIR}/bin)
dviglo_set_bin_dir(${target_name} ${bin_dir})

# Рабочая папка с ресурсами при отладке
set_property(TARGET ${target_name} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${bin_dir})

# Создаём ссылку на папку с ресурсами (требует админских прав)
file(MAKE_DIRECTORY ${bin_dir})
dviglo_create_symlink(${CMAKE_CURRENT_SOURCE_DIR}/bitmap_font_generator_data ${bin_dir}/bitmap_font_generator_data)

# Заставляем VS отображать дерево каталогов
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_files})
