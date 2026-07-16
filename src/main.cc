#include <cstdlib>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace fs = std::filesystem;

// Create a file only when it does not already exist.
void write_file_if_absent(const fs::path &filepath, std::string_view content)
{
    if (fs::exists(filepath))
    {
        std::cout << "[Skipped] Existing file preserved: " << filepath.string() << '\n';
        return;
    }

    std::ofstream out(filepath, std::ios::out);
    if (!out)
    {
        throw std::runtime_error("Failed to open file for writing: " + filepath.string());
    }

    out << content;
    if (!out)
    {
        throw std::runtime_error("Failed to write file: " + filepath.string());
    }

    std::cout << "[Created] " << filepath.string() << '\n';
}

std::string sanitize_project_name(std::string_view name)
{
    std::string result;

    for (unsigned char ch : name)
    {
        if (std::isalnum(ch))
        {
            result.push_back(static_cast<char>(std::tolower(ch)));
        }
        else if (ch == '-' || ch == '_' || ch == ' ')
        {
            result.push_back('_');
        }
    }

    if (result.empty())
    {
        result = "cpp23_project";
    }

    if (std::isdigit(static_cast<unsigned char>(result.front())))
    {
        result.insert(result.begin(), '_');
    }

    return result;
}

std::string current_folder_project_name()
{
    const fs::path current = fs::current_path();
    std::string folder_name = current.filename().string();

    if (folder_name.empty())
    {
        folder_name = "cpp23_project";
    }

    return sanitize_project_name(folder_name);
}

void ensure_directory(const fs::path &dirpath)
{
    if (fs::exists(dirpath))
    {
        if (!fs::is_directory(dirpath))
        {
            throw std::runtime_error("Path exists but is not a directory: " + dirpath.string());
        }
        std::cout << "[Skipped] Directory already exists: " << dirpath.string() << '\n';
        return;
    }

    fs::create_directories(dirpath);
    std::cout << "[Created] Directory: " << dirpath.string() << '\n';
}

int main()
{
    try
    {
        const fs::path project_root = ".";
        const fs::path src_dir = project_root / "src";
        const fs::path vscode_dir = project_root / ".vscode";
        const std::string project_name = current_folder_project_name();

        std::cout << "[Project] " << project_name << '\n';

        ensure_directory(src_dir);
        ensure_directory(vscode_dir);

        constexpr std::string_view main_content = R"(#include <iostream>

int main() {
    std::cout << "Hello C++23\n";
    return 0;
}
)";

        const std::string cmake_content =
            "cmake_minimum_required(VERSION 3.20)\n"
            "project(" +
            project_name + " LANGUAGES CXX)\n"
                           "\n"
                           "set(CMAKE_CXX_STANDARD 23)\n"
                           "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n"
                           "set(CMAKE_CXX_EXTENSIONS OFF)\n"
                           "\n"
                           "add_executable(" +
            project_name + " src/main.cc)\n";

        const std::string readme_content =
            "# " + project_name + "\n"
                                  "\n"
                                  "```sh\n"
                                  "code --install-extension ms-vscode.cpptools-extension-pack\n"
                                  "cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug\n"
                                  "cmake --build build\n"
                                  "```\n"
                                  "sudo apt update && sudo apt install -y gdb\n"
                                  "\n"
                                  "1. `Ctrl+Shift+P` or F1 -> CMake: Select a Kit -> choose GCC (or Clang if you use it)\n"
                                  "2. `Ctrl+Shift+P` or F1 -> CMake: Configure\n"
                                  "3. `Ctrl+Shift+P` or F1 -> CMake: Build\n"
                                  "4. `Ctrl+Shift+P` or F1 -> CMake: Run ... or Press `F5` to build and debug in VS Code\n";

        const std::string launch_json =
            "{\n"
            "  \"version\": \"0.2.0\",\n"
            "  \"configurations\": [\n"
            "    {\n"
            "      \"name\": \"Debug " +
            project_name + "\",\n"
                           "      \"type\": \"cppdbg\",\n"
                           "      \"request\": \"launch\",\n"
                           "      \"program\": \"${workspaceFolder}/build/" +
            project_name + "\",\n"
                           "      \"args\": [],\n"
                           "      \"stopAtEntry\": false,\n"
                           "      \"cwd\": \"${workspaceFolder}\",\n"
                           "      \"environment\": [],\n"
                           "      \"externalConsole\": false,\n"
                           "      \"MIMode\": \"gdb\",\n"
                           "      \"preLaunchTask\": \"CMake Build Debug\"\n"
                           "    }\n"
                           "  ]\n"
                           "}\n";

        const std::string tasks_json =
            "{\n"
            "  \"version\": \"2.0.0\",\n"
            "  \"tasks\": [\n"
            "    {\n"
            "      \"label\": \"CMake Build Debug\",\n"
            "      \"type\": \"shell\",\n"
            "      \"command\": \"cmake\",\n"
            "      \"args\": [\n"
            "        \"-S\", \".\",\n"
            "        \"-B\", \"build\",\n"
            "        \"-DCMAKE_BUILD_TYPE=Debug\"\n"
            "      ],\n"
            "      \"group\": \"build\",\n"
            "      \"problemMatcher\": []\n"
            "    },\n"
            "    {\n"
            "      \"label\": \"CMake Compile Debug\",\n"
            "      \"type\": \"shell\",\n"
            "      \"command\": \"cmake\",\n"
            "      \"args\": [\n"
            "        \"--build\", \"build\"\n"
            "      ],\n"
            "      \"dependsOn\": \"CMake Build Debug\",\n"
            "      \"group\": {\n"
            "        \"kind\": \"build\",\n"
            "        \"isDefault\": true\n"
            "      },\n"
            "      \"problemMatcher\": [\"$gcc\"]\n"
            "    }\n"
            "  ]\n"
            "}\n";

        // Use the compile task as the debugger's pre-launch task.
        const std::string fixed_launch_json =
            "{\n"
            "  \"version\": \"0.2.0\",\n"
            "  \"configurations\": [\n"
            "    {\n"
            "      \"name\": \"Debug " +
            project_name + "\",\n"
                           "      \"type\": \"cppdbg\",\n"
                           "      \"request\": \"launch\",\n"
                           "      \"program\": \"${workspaceFolder}/build/" +
            project_name + "\",\n"
                           "      \"args\": [],\n"
                           "      \"stopAtEntry\": false,\n"
                           "      \"cwd\": \"${workspaceFolder}\",\n"
                           "      \"environment\": [],\n"
                           "      \"externalConsole\": false,\n"
                           "      \"MIMode\": \"gdb\",\n"
                           "      \"preLaunchTask\": \"CMake Compile Debug\"\n"
                           "    }\n"
                           "  ]\n"
                           "}\n";

        write_file_if_absent(src_dir / "main.cc", main_content);
        write_file_if_absent(project_root / "CMakeLists.txt", cmake_content);
        write_file_if_absent(project_root / "README.md", readme_content);
        write_file_if_absent(vscode_dir / "launch.json", fixed_launch_json);
        write_file_if_absent(vscode_dir / "tasks.json", tasks_json);

        std::cout << "\nC++23 project scaffold ready.\n";

        // Launch VS Code as a convenience; failure is non-fatal.
        const int rc = std::system("code . src/main.cc");
        if (rc != 0)
        {
            std::cerr << "Warning: failed to launch VS Code.\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
