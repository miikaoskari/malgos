add_languages('cpp', native: false, required: true)

benchmark_main_dep = dependency('benchmark_main')

benches = [
    'bench_intrusive_hashtable',
]

foreach name : benches
    executable(
        name,
        name + '.cpp',
        dependencies: [malgos_dep, benchmark_main_dep],
        # the container headers use statement expressions and typeof, so the
        # benchmarks need the GNU dialect rather than strict ISO C++
        override_options: ['cpp_std=gnu++17'],
    )
endforeach
