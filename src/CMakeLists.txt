add_library(malgos STATIC)

target_sources(malgos
    PRIVATE
        hashmap.c
        piecetable.cpp
)

target_include_directories(malgos
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(malgos
    PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
)
