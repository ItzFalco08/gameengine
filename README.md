# How to Run

## Prerequisites
- `Ninja` build system
- `CMake`
- `MSVC` 2022 C++ Compiler

## Build Protocol

**1. Generate Ninja build files**
```bash
cmake -G Ninja -B build
```

**2. Compile/Build**
```bash
cmake --build build
```

**3. Run**
```bash
cd build
./engine.exe
```