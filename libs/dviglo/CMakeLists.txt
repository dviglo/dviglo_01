if(dviglo_build_all_libs)
    # Добавляем все подпапки
    dviglo_add_all_subdirectories()
else()
    # Добавляем только библиотеки, которые используются в других таргетах
    dviglo_add_all_subdirectories(EXCLUDE_FROM_ALL)
endif()
