add_library(malgos STATIC)

target_sources(malgos
    PRIVATE
        intrusive/hashtable.c
        common/hash.c
)

target_include_directories(malgos
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)
