cmake_minimum_required(VERSION 3.21)

project(malgos C)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_C_STANDARD 23)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_C_EXTENSIONS OFF)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    option(MALGOS_ENABLE_SANITIZERS "enable asan and ubsan" OFF)
    option(MALGOS_BUILD_BENCHMARKS "build benchmarks (requires a cpp compiler)" OFF)
endif()

add_subdirectory(src)

set_target_properties(malgos PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)

if(PROJECT_IS_TOP_LEVEL)
    target_compile_options(malgos
        PRIVATE
            $<$<OR:$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:GNU>>:
                -Wall
                -Wextra
                -Wpedantic
            >

            $<$<C_COMPILER_ID:MSVC>:
                /W4
            >
    )

    if(MALGOS_ENABLE_SANITIZERS)
        target_compile_options(malgos PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(malgos PRIVATE -fsanitize=address,undefined)
    endif()

    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()

    if(MALGOS_BUILD_BENCHMARKS)
        enable_language(CXX)
        add_subdirectory(bench)
    endif()
endif()
