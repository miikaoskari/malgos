add_library(malgos STATIC)
add_library(malgos::malgos ALIAS malgos)

target_sources(malgos
    PRIVATE
        intrusive/hashtable.c
        intrusive/dlist.c
        intrusive/slist.c
)

target_include_directories(malgos
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)
