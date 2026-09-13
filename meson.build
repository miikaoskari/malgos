project(
    'malgos',
    'c',
    version: '0.1.0',
    license: 'MIT',
    meson_version: '>= 1.3.0',
    default_options: [
        'buildtype=release',
        'c_std=c23,c2x',
        'default_library=static',
        'warning_level=2',
    ],
)

cc = meson.get_compiler('c')

malgos_warnings = cc.get_supported_arguments(
    '-Wshadow',
)

malgos_target_options = [
    'c_std=c99',
    'warning_level=3',
    'werror=true',
]

malgos_inc = include_directories('include')

# apple's clang accepts -fsanitize=fuzzer but ships no libFuzzer runtime, so
# probe the linker instead of trusting the compiler id.
fuzzers_opt = get_option('fuzz')
build_fuzzers = false
if fuzzers_opt.allowed()
    build_fuzzers = cc.links(
        '''int LLVMFuzzerTestOneInput(const unsigned char *data, unsigned long size)
           { (void)data; (void)size; return 0; }''',
        args: '-fsanitize=fuzzer',
        name: 'libFuzzer',
    )

    if fuzzers_opt.enabled() and not build_fuzzers
        error(
            'the fuzzers option requires a clang with the libFuzzer runtime, but '
            + cc.get_id() + ' ' + cc.version() + ' cannot link -fsanitize=fuzzer',
        )
    endif
endif

if build_fuzzers
    if not meson.version().version_compare('>= 1.8.0')
        error(
            'building the fuzz targets needs meson >= 1.8.0, which is the first '
            + 'release whose b_sanitize option accepts fuzzer-no-link',
        )
    endif

    if 'fuzzer-no-link' not in get_option('b_sanitize')
        error(
            'the fuzz targets need the library instrumented for coverage; '
            + 'configure with -Db_sanitize=address,undefined,fuzzer-no-link '
            + '-Db_lundef=false',
        )
    endif

    if not get_option('debug')
        warning(
            'this fuzzing build has no debug info, so libFuzzer cannot symbolize '
            + 'the library in crash reports or -print_coverage output; configure '
            + 'with --buildtype=debugoptimized',
        )
    endif
endif

conflicts_with_fuzzers = 'cannot be built in a fuzzing build directory, because they would link the instrumented library without a SanitizerCoverage runtime; configure a separate build directory for them'

build_tests = get_option('test').disable_auto_if(meson.is_subproject()).disable_if(
    build_fuzzers,
    error_message: 'test ' + conflicts_with_fuzzers,
)

build_benchmarks = get_option('bench').disable_if(
    build_fuzzers,
    error_message: 'bench ' + conflicts_with_fuzzers,
)

subdir('src')

malgos_dep = declare_dependency(
    link_with: malgos_lib,
    include_directories: malgos_inc,
)

meson.override_dependency('malgos', malgos_dep)

if build_tests.allowed()
    subdir('tests')
endif

if build_benchmarks.allowed()
    subdir('bench')
endif

if build_fuzzers
    subdir('fuzz')
endif

install_subdir(
    'include/malgos',
    install_dir: get_option('includedir'),
)

import('pkgconfig').generate(
    malgos_lib,
    name: 'malgos',
    description: 'zero-allocation containers for freestanding environments',
)

summary(
    {
        'tests': build_tests.allowed(),
        'benchmarks': build_benchmarks.allowed(),
        'fuzzers': build_fuzzers,
        'sanitizers': get_option('b_sanitize'),
    },
    section: 'malgos',
    bool_yn: true,
)

