add_executable(tests
    test_hashmap.cpp
    test_piecetable.cpp
)

target_include_directories(tests PRIVATE ${OPENSSL_INCLUDE_DIR})

target_link_libraries(tests
    PRIVATE
        malgos
        GTest::gtest_main
        OpenSSL::SSL
        OpenSSL::Crypto
)

include(GoogleTest)
include(CTest)
gtest_discover_tests(tests)