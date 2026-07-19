add_library(malgos STATIC)

target_sources(malgos
    PRIVATE
        intrusive/hashtable.c
        intrusive/dlist.c
)

target_include_directories(malgos
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)
