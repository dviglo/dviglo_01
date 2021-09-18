# Название таргета
set(target_name imgui_01)

# Создаём список файлов.
# Не используем рекурсию, так как файлы из папки repo/backends не используются в данной библиотеке
# (но используются в других)
file(GLOB source_files repo/*.cpp repo/*.h)

# Создаём статическую библиотеку
add_library(${target_name} STATIC ${source_files})

# Делаем заголовочные файлы доступными таргетам, которые используют текущую библиотеку
target_include_directories(${target_name} INTERFACE repo)

# Заставляем VS отображать дерево каталогов
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/repo FILES ${source_files})
