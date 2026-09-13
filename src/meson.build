malgos_sources = files(
    'intrusive/hashtable.c',
    'intrusive/dlist.c',
    'intrusive/slist.c',
)

malgos_lib = library(
    'malgos',
    malgos_sources,
    include_directories: malgos_inc,
    c_args: malgos_warnings,
    override_options: malgos_target_options,
    install: true,
)
