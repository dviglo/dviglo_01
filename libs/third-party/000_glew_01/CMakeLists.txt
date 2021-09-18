# Название таргета
set(target_name glew_01)

# Создаём список файлов
file(GLOB_RECURSE source_files repo/*.c repo/*.h)

# Создаём статическую библиотеку
add_library(${target_name} STATIC ${source_files})

# Дефайны для библиотеки и для таргетов, которые используют текущую библиотеку
target_compile_definitions(${target_name} PUBLIC GLEW_STATIC GLEW_NO_GLU)

# Делаем заголовочные файлы доступными библиотеке и таргетам, которые используют текущую библиотеку
target_include_directories(${target_name} PUBLIC repo/include)

# Подключаем OpenGL к таргетам, которые используют текущую библиотеку
find_package(OpenGL)
target_link_libraries(${target_name} INTERFACE ${OPENGL_gl_LIBRARY})
target_include_directories(${target_name} INTERFACE ${OPENGL_INCLUDE_DIR})

# Заставляем VS отображать дерево каталогов
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/repo FILES ${source_files})
