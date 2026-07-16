# C++23 Project Scaffold Generator

A small C++23 utility that bootstraps a minimal CMake-based C++ project in the current directory.

It creates a ready-to-build layout with:

- `src/main.cc`
- `CMakeLists.txt`
- `README.md`
- `.vscode/launch.json`
- `.vscode/tasks.json`

Existing files are preserved and never overwritten.

## Features

- Uses the current folder name as the project name
- Sanitizes the project name for safe CMake target naming
- Creates missing directories automatically
- Generates a simple Hello World entry point
- Adds VS Code debug and build tasks
- Opens the project in VS Code after generation

## Requirements

- C++23-compatible compiler
- CMake 3.20 or newer
- VS Code (optional)
- `gdb` for debugging on Linux
- A system `code` command if you want the generator to open VS Code automatically

## Build the Generator
```sh
g++ -std=c++23 -O2 -Wall -Wextra -pedantic src/main.cc -o newcc
```

The tool will automatically generate the files and attempt to launch VS Code in the project directory.

## Development Requirements
- **Compiler:** GCC with C++23 support.
- **Tools:** `cmake`, `vscode` (for auto-launch integration).

## License
MIT
