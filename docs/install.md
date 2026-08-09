# Installation

To build malgos you need the following:
- CMake
- C compiler (gcc, clang, msvc)

## Building from source

Build a static library:

```bash
git clone https://github.com/miikaoskari/malgos.git
cd malgos
cmake -B build -S .
cmake --build build
```

## CMake FetchContent

Include in you CMakeLists.txt

```cmake
include(FetchContent)
FetchContent_Declare(
    malgos
    GIT_REPOSITORY https://github.com/miikaoskari/malgos.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(malgos)

target_link_libraries(your_nice_binary PRIVATE malgos)
```
