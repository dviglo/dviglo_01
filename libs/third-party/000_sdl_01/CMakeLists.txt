# Эта переменная используется в repo/CMakeLists.txt.
# Не компилируем динамическую версию библиотеки, а только статическую
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)

# Обходим эту ошибку: https://github.com/libsdl-org/SDL/issues/4258
set(OPT_DEF_LIBC ON)

add_subdirectory(repo)
