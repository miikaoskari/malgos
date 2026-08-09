# Testing

This project uses [Unity](https://github.com/ThrowTheSwitch/Unity) by ThrowTheSwitch.
which is wired up into CMake ctest. 

The unit tests can also be built with UBSan and ASan by defining `-D ENABLE_SANITIZERS=ON` in CMake configuration.

To run the unit tests first build the project

```bash
cmake -B build -S . -D ENABLE_SANITIZERS=ON

cmake --build build
```

Then run ctest in the build directory

```bash
cd build

ctest
```
