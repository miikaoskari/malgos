![stack](/assets/stack.png) **malgos**

algorithm and data structure library with c and c++

## building

to build the project:
```bash
cmake -B build -S .
cmake --build build
```

## testing

this project uses [Unity](https://github.com/ThrowTheSwitch/Unity) by ThrowTheSwitch.

to run the unit tests first build the project and then run:
```bash
cd build
ctest
```
