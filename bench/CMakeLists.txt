include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.1
)
FetchContent_MakeAvailable(benchmark)

set(benches
    bench_intrusive_hashtable.cpp
)

foreach(source IN LISTS benches)
    get_filename_component(name ${source} NAME_WE)

    add_executable(${name} ${source})

    target_link_libraries(${name}
        PRIVATE
            malgos
            benchmark::benchmark_main
    )
endforeach()
