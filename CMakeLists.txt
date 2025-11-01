cmake_minimum_required(VERSION 3.10)

project(malgos)

find_package(OpenSSL REQUIRED)

add_library(malgos STATIC)

include(FetchContent)
FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity
    GIT_TAG        d89dafa41334ec4c55fafcf0b7cdb77a04148d43
)

FetchContent_Declare(
    cmock
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/CMock
    GIT_TAG        94dcd39765859fb681d500307bbe1b5da4851df2
)

FetchContent_MakeAvailable(unity cmock)

target_sources(malgos
    PRIVATE
        src/hashmap.c
)

enable_testing()
add_executable(test_hashmap tests/test_hashmap.c)

target_link_libraries(test_hashmap
    PRIVATE
        malgos
        unity
)

target_include_directories(test_hashmap
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${unity_SOURCE_DIR}/src
        ${cmock_SOURCE_DIR}/src
)

add_test(NAME test_hashmap COMMAND test_hashmap)

target_include_directories(malgos
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(malgos
    PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
)
