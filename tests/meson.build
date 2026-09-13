unity_dep = dependency('unity')

tests = [
    'test_intrusive_hashtable',
    'test_intrusive_dlist',
    'test_intrusive_slist',
]

foreach name : tests
    exe = executable(
        name,
        name + '.c',
        dependencies: [malgos_dep, unity_dep],
    )

    test(name, exe)
endforeach
