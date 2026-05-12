#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <cstdlib>
#include <sstream>
#include <filesystem>
#include <unistd.h> //fork, exec, execvp, access
#include <sys/wait.h> //waitpid


bool is_builtin_command(const std::string& command) {
  static const std::unordered_set<std::string> builtins = {
    "exit",
    "echo",
    "type",
    "pwd"
};

  return builtins.find(command) != builtins.end();
}

bool is_executable_file(const std::filesystem::path& path) {
    return std::filesystem::exists(path)
        && std::filesystem::is_regular_file(path)
        && access(path.c_str(), X_OK) == 0;
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

std::string find_executable(const std::string& command) {
  const char* path_env = std::getenv("PATH");

  if (path_env == nullptr) {
    return "";
  }

  std::vector<std::string> dirs = split_path(path_env);

  for (const std::string& dir : dirs) {
    std::filesystem::path full_path = std::filesystem::path(dir) / command;

    if (is_executable_file(full_path)) {
      return full_path.string();
    }
  }

  return "";
}

std::vector<std::string> split_input(const std::string& input) {
  /*
    将输入划分为：[命令, arg1, arg2, ...]
  */
  std::vector<std::string> args;
  std::stringstream ss(input);
  std::string arg;

  while (ss >> arg) {
    args.push_back(arg);
  }
  
  return args;
}

std::vector<char*> make_argv(std::vector<std::string>& args) {
  /*
    将vector<string>转换为c风格的 char* const argv[]，以便使用execv
  */
  std::vector<char*> argv;

  for (std::string& arg: args) {
    argv.push_back(arg.data());
  }

  argv.push_back(nullptr);

  return argv;
}

void execute_external_command(std::vector<std::string> args) {
  if (args.empty()) {
    return;
  }

  const std::string& command = args[0];

  std::string executable_path = find_executable(command);

  if (executable_path.empty()) {
    std::cout << command << ": command not found" << std::endl;
    return;
  }

  pid_t pid = fork();

  if (pid < 0) {
    std::cerr << "fork failed" << std::endl;
    return;
  }

  if (pid == 0) {
    std::vector<char*> argv = make_argv(args);

    execv(executable_path.c_str(), argv.data());

    std::cerr << command << ": execution failed" << std::endl;
    _exit(1);
  }

  int status = 0;

  if (waitpid(pid, &status, 0) == -1) {
    std::cerr << "waitpid failed" << std::endl;
  }
}


void handle_type_command(const std::string& command) {
  if (is_builtin_command(command)) {
    std::cout << command << " is a shell builtin" << std::endl;
    return;
  }

  std::string executable_path = find_executable(command);

  if (!executable_path.empty()) {
    std::cout << command << " is " << executable_path << std::endl;
    return;
  }

  std::cout << command << ": not found" << std::endl;
}

void handle_echo_command(const std::vector<std::string>& args) {
  for (size_t i = 1; i < args.size(); i++) {
    if (i > 1) {
      std::cout << " ";
    }

    std::cout << args[i];
  }

  std::cout << std::endl;
}

void handle_pwd_command(const std::vector<std::string>& args) {
  std::cout << std::filesystem::current_path().string() << std::endl;
}

void handle_input(const std::string& input) {
    std::vector<std::string> args = split_input(input);

    if (args.empty()) {
        return;
    }

    const std::string& command = args[0];

    if (command == "exit") {
        std::exit(0);
    }

    if (command == "echo") {
      handle_echo_command(args);
      return;
    }

    if (command == "type") {
        if (args.size() >= 2) {
            handle_type_command(args[1]);
        }
        return;
    }

    if (command == "pwd") {
      handle_pwd_command(args);
      return;
    }

    execute_external_command(args);
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