cmake_minimum_required(VERSION 3.10)

project(malgos)

find_package(OpenSSL REQUIRED)

add_library(malgos STATIC
)

target_sources(malgos
    PRIVATE
        src/hashmap.c
)

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
