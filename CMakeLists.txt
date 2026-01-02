cmake_minimum_required(VERSION 3.10)

project(malgos CXX C)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)

add_subdirectory(src)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.17.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory(tests)
