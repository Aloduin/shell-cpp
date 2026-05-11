#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <cstdlib>
#include <sstream>
#include <filesystem>
#include <unistd.h>


bool is_builtin_command(const std::string& command) {
  static const std::unordered_set<std::string> builtins = {
    "exit",
    "echo",
    "type",
};

  if (builtins.find(command) != builtins.end()) return true;
  return false;
}

std::vector<std::string> split_path(const std::string& path_env) {
  std::vector<std::string> dirs;
  std::stringstream ss(path_env);
  std::string dir;

  while (std::getline(ss, dir, ':')) {
    if (!dir.empty()) {
      dirs.push_back(dir);
    }
  }
  return dirs;
}

bool is_executable_file(const std::filesystem::path& path) {
  return (
    std::filesystem::exists(path)
    && std::filesystem::is_regular_file(path)
    && access(path.c_str(), X_OK) == 0
  );
}

void handle_type_command(const std::string& command) {
  if (is_builtin_command(command)) {
    std::cout << command << " is a shell builtin" << std::endl;
    return;
  }

  const char* path_env = std::getenv("PATH");
  if (path_env == nullptr) {
    std::cout << command << ": not found" << std::endl;
    return;
  }

  std::vector<std::string> dirs = split_path(path_env);
  for (const std::string& dir : dirs) {
    std::filesystem::path full_path = std::filesystem::path(dir) / command;

    if (is_executable_file(full_path)) {
      std::cout << command << " is " << full_path.string() << std::endl;
      return;
    }
  }

  std::cout << command << ": not found" << std::endl;
}

void handle_echo_command(std::stringstream& ss) {
    std::string rest;
    std::getline(ss, rest);

    if (!rest.empty() && rest[0] == ' ') {
        rest.erase(0, 1);
    }

    std::cout << rest << std::endl;
}

void handle_input(const std::string& input) {
    std::stringstream ss(input);

    std::string command;
    ss >> command;

    if (command.empty()) {
        return;
    }

    if (command == "exit") {
        std::exit(0);
    }

    if (command == "echo") {
        handle_echo_command(ss);
        return;
    }

    if (command == "type") {
        std::string target;
        ss >> target;

        if (!target.empty()) {
            handle_type_command(target);
        }

        return;
    }

    std::cout << command << ": command not found" << std::endl;
}

int main() {
    std::string input;

    while (true) {
        std::cout << "$ ";
        std::cout.flush();

        if (!std::getline(std::cin, input)) {
            break;
        }

        handle_input(input);
    }

    return 0;
}