# newcc
Create new C++17 CMake project


A minimal, zero-dependency C++17 project generator. This utility creates a standard project structure and basic boilerplate, optimized for VS Code + CMake workflows on Linux.

## Overview
This tool automates the repetitive setup phase of C++ development. It generates the necessary directory structure and configuration files to get a modern C++17 project up and running in seconds.

## What it generates
- `src/main.cc`: A "Hello World" entry point.
- `CMakeLists.txt`: A standard CMake configuration enforcing C++17.
- `README.md`: Quick-start instructions for VS Code.

## Usage
Compile and run the generator:
```bash
g++ -std=c++17 src/main.cc -o newcc

./newcc
```

The tool will automatically generate the files and attempt to launch VS Code in the project directory.

## Development Requirements
- **Compiler:** GCC or Clang with C++17 support.
- **Tools:** `cmake`, `vscode` (optional, for auto-launch integration).

## License
MIT
