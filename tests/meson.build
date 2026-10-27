unity_dep = dependency(
    'unity',
    default_options: ['support_double=true'],
)

unity_args = ['-DUNITY_INCLUDE_DOUBLE']

tests = [
    'test_intrusive_hashtable',
    'test_intrusive_dlist',
    'test_intrusive_slist',
    'test_allocators_arena',
]

foreach name : tests
    exe = executable(
        name,
        name + '.c',
        c_args: unity_args,
        dependencies: [malgos_dep, unity_dep],
    )

    test(name, exe)
endforeach
