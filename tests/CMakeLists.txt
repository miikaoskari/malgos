include(FetchContent)

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.6.1
)
FetchContent_MakeAvailable(unity)

set(tests
    test_intrusive_hashtable.c
    test_intrusive_dlist.c
)

foreach(source IN LISTS tests)
    get_filename_component(name ${source} NAME_WE)

    add_executable(${name} ${source})

    target_link_libraries(${name}
        PRIVATE
            malgos
            unity
    )

    if(ENABLE_SANITIZERS)
        target_compile_options(${name} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${name} PRIVATE -fsanitize=address,undefined)
    endif()

    add_test(NAME ${name} COMMAND ${name})
endforeach()
