add_library(malgos STATIC)

target_sources(malgos
    PRIVATE
        intrusive/hashtable.c
        intrusive/dlist.c
)

target_include_directories(malgos
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

set_target_properties(malgos PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)

target_compile_options(malgos
    PRIVATE
        $<$<C_COMPILER_ID:Clang>:
            -Wall
            -Wextra
            -Wpedantic
        >

        $<$<C_COMPILER_ID:GNU>:
            -Wall
            -Wextra
            -Wpedantic
        >

        $<$<C_COMPILER_ID:MSVC>:
            /W4
        >
)

if(ENABLE_SANITIZERS)
    target_compile_options(malgos PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(malgos PRIVATE -fsanitize=address,undefined)
endif()
