include(FetchContent)

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.6.1
)
FetchContent_MakeAvailable(unity)

add_library(Unity STATIC ${unity_SOURCE_DIR}/src/unity.c)

add_executable(tests
    test_intrusive_hashtable.c
)

target_include_directories(tests PRIVATE
    ${unity_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(tests
    PRIVATE
        malgos
        Unity
)

include(CTest)
add_test(NAME tests COMMAND tests)
