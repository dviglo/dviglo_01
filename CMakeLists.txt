cmake_minimum_required(VERSION 3.21.2)

project(dviglo)

# Требуем, чтобы компилятор поддерживал стандарт C++20

#set(CMAKE_CXX_STANDARD 20)

#https://github.com/microsoft/STL/issues/1814#issuecomment-914784312
if (MSVC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.29.30129 AND CMAKE_VERSION VERSION_GREATER 3.20.3)
    # this change happened in CMake 3.20.4
    set(CMAKE_CXX_STANDARD 23) # /std:c++latest - unlocks the non stable cpp20 features. For new 16.11 versions
else ()
    set(CMAKE_CXX_STANDARD 20) # /std:c++latest for msvc and -std=c++20 for everyone else.
endif ()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Включаем поддержку модулей в Студии
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /experimental:module")

# Получаем доступ к функциям
include(${CMAKE_SOURCE_DIR}/cmake/dviglo_utils.cmake)

add_subdirectory(libs)
add_subdirectory(samples)
