include(FetchContent)

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.6.1
)
FetchContent_MakeAvailable(unity)

add_library(Unity STATIC ${unity_SOURCE_DIR}/src/unity.c)

set(tests
    test_intrusive_hashtable.c
    test_intrusive_dlist.c
)

foreach(source IN LISTS tests)
    get_filename_component(name ${source} NAME_WE)

    add_executable(${name} ${source})

    target_include_directories(${name} PRIVATE
        ${unity_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
    )

    target_link_libraries(${name}
        PRIVATE
            malgos
            Unity
    )

    add_test(NAME ${name} COMMAND ${name})
endforeach()
