#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string_view>

namespace fs = std::filesystem;

// Helper function to write content to a file
void write_file(const fs::path& filepath, std::string_view content) {
    std::ofstream out(filepath, std::ios::out | std::ios::trunc);
    if (!out) {
        throw std::runtime_error("Failed to open file for writing: " + filepath.string());
    }
    out << content;
    if (!out) {
        throw std::runtime_error("Failed to write file: " + filepath.string());
    }
    std::cout << "[Created] " << filepath.string() << '\n';
}

int main() {
    try {
        const fs::path project_root = ".";
        const fs::path src_dir = project_root / "src";

        // Create directory structure
        if (!fs::exists(src_dir)) {
            fs::create_directories(src_dir);
            std::cout << "[Created] Directory: " << src_dir.string() << '\n';
        }

        // File contents
        constexpr std::string_view main_content = R"(
#include <iostream>

int main() {
    std::cout << "Hello C++17\n";
    return 0;
}
)";

        constexpr std::string_view cmake_content = R"(
cmake_minimum_required(VERSION 3.16)
project(hello_cpp17 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(hello src/main.cc)
)";

        constexpr std::string_view readme_content = R"(
1. `Ctrl+Shift+P` or F1 → **CMake: Select a Kit** → choose **GCC** (or Clang if you use it)
2. `Ctrl+Shift+P` or F1 → **CMake: Configure**
3. `Ctrl+Shift+P` or F1 → **CMake: Build**
4. `Ctrl+Shift+P` or F1 → **CMake: Run Without Debugging**
)";

        // Write files
        write_file(src_dir / "main.cc", main_content);
        write_file(project_root / "CMakeLists.txt", cmake_content);
        write_file(project_root / "README.md", readme_content);

        std::cout << "\nC++17 project scaffold generated successfully!\n";

        // Launch VS Code at the end
        const int rc = std::system("code . src/main.cc");

        if (rc != 0) {
            std::cerr << "Warning: failed to launch VS Code, exit code: " << rc << '\n';
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}