cmake_minimum_required(VERSION 3.10)

project(malgos CXX C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

include(FetchContent)

enable_testing()

add_subdirectory(tests)
