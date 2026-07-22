cmake_minimum_required(VERSION 3.21)

project(malgos C)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_C_STANDARD 23)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_C_EXTENSIONS OFF)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

option(ENABLE_SANITIZERS "enable asan and ubsan" OFF)

add_subdirectory(src)

if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()
