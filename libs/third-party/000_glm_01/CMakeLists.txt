# Название таргета
set(target_name glm_01)

# Создаём список файлов
file(GLOB_RECURSE source_files repo/*.h repo/*.hpp)

# Создаём интерфейсную библиотеку. Интерфейсная библиотека не компилируется,
# так как состоит только из заголовочных файлов
add_library(${target_name} INTERFACE ${source_files})

# Делаем заголовочные файлы доступными таргетам, которые используют текущую библиотеку
target_include_directories(${target_name} INTERFACE repo)

# Заставляем VS отображать дерево каталогов
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/repo FILES ${source_files})
