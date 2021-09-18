# Название таргета
set(target_name imgui_sdl_gl3_01)

# Список файлов
set(backends_dir ${CMAKE_SOURCE_DIR}/libs/third-party/000_imgui_01/repo/backends)
set(source_files ${backends_dir}/imgui_impl_sdl.cpp
                 ${backends_dir}/imgui_impl_sdl.h
                 ${backends_dir}/imgui_impl_opengl3.cpp
                 ${backends_dir}/imgui_impl_opengl3.h
                 ${backends_dir}/imgui_impl_opengl3_loader.h)

# Создаём статическую библиотеку
add_library(${target_name} STATIC ${source_files})

# Делаем заголовочные файлы доступными таргетам, которые используют текущую библиотеку
target_include_directories(${target_name} INTERFACE ${backends_dir})

# Подключаем библиотеки к текущей библиотеке и к таргетам, использующим текущую библиотеку
target_link_libraries(${target_name} PUBLIC imgui_01 SDL2main SDL2-static)

# Глубина зависимости: 001
# 000_imgui_01, 000_sdl_01

# Подключаем OpenGL к таргетам, которые используют текущую библиотеку
find_package(OpenGL)
target_link_libraries(${target_name} INTERFACE ${OPENGL_gl_LIBRARY})
target_include_directories(${target_name} INTERFACE ${OPENGL_INCLUDE_DIR})

# Заставляем VS отображать дерево каталогов
source_group(TREE ${backends_dir} FILES ${source_files})
