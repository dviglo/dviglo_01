# Добавляет все поддиректории.
# Использование: dviglo_add_all_subdirectories() или dviglo_add_all_subdirectories(EXCLUDE_FROM_ALL)
function(dviglo_add_all_subdirectories)
    # Список файлов и подпапок
    file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

    foreach(child ${children})
        # Если не директория, то пропускаем
        if(NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
            continue()
        endif()

        # Функция dviglo_add_all_subdirectories() ожидает 0 аргументов.
        # Все лишние аргументы помещаются в ARGN
        add_subdirectory(${child} ${ARGN})
    endforeach()
endfunction()


# Куда будет помещён скомпилированный исполняемый файл
function(dviglo_set_bin_dir target_name bin_dir)
    # Для Visual Studio
    foreach(config_name ${CMAKE_CONFIGURATION_TYPES}) # Release, Debug и т.д.
        string(TOUPPER ${config_name} config_name)
        set_property(TARGET ${target_name} PROPERTY RUNTIME_OUTPUT_DIRECTORY_${config_name} ${bin_dir})
    endforeach()
endfunction()


# Создаёт символическую ссылку для папки. Требует админских прав в Windows. Если создать символическую
# ссылку не удалось, то копирует папку.
# Существует функция file(CREATE_LINK ${from} ${to} COPY_ON_ERROR SYMBOLIC), однако в случае неудачи она копирует папку без содержимого
function(dviglo_create_symlink from to)
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${from} ${to} RESULT_VARIABLE result)
    if(NOT result EQUAL "0")
        message("Не удалось создать символическую ссылку. Нужно запускать cmake от Админа. Копируем вместо создания ссылки")
        execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory  ${from} ${to})
    endif()
endfunction()


# Приложение нужно создавать с помощью этой функции. Функция учитывает опцию dviglo_win_console
# и создаёт необходимый дефайн
function(dviglo_add_executable target_name source_files)
    # Если пользователь при передаче в макрос списка ${source_files} не заключил его в кавычки,
    # то элементы списка будут переданы как отдельные аргументы. Восстанавливаем список
    list(APPEND source_files ${ARGN})

    if(WIN32)
        if(dviglo_win_console)
            # Создаём конольное приложение Windows
            add_executable(${target_name} ${source_files})

            # Определяем дефайн
            add_definitions(-D DVIGLO_WIN_CONSOLE)
        else()
            # Создаём десктопное приложение Windows
            add_executable(${target_name} WIN32 ${source_files})
        endif()
    else() # Linux
        # Создаём приложение
        add_executable(${target_name} ${source_files})
    endif()
endfunction()

# Все ОС для точки входа по умолчанию используют функцию main (в том числе Windows для консольного приложения)
# и только десктопное приложение Windows испольузет точку входа WinMain.
# В VS тип приложения (консольное или десктопное) указывается в `Свойства проекта` > `Linker` > `System` > `SubSystem`,
# однако можно указать и в исходнике с помощью `#pragma comment(linker, "/subsystem:console")` и `#pragma comment(linker, "/subsystem:windows")`.
# Также VS определяет дефайн `_CONSOLE` при создании консольного приложения
# и дефайн `_WINDOWS` при создании десктропного приложения, но эти дефайны не документированы, ни на что не влияют и не используются в h-файлах Windows SDK.
# Видимо, предназначены только для пользователей.
# CMake при создании и консольного и десктропного приложения всегда определяет дефайн `_WINDOWS`: <https://cmake.org/Bug/view.php?id=13292>.
# Макрос выше опредляет дефайн `DVIGLO_WIN_CONSOLE` если создаёт консольное приложение Windows.
