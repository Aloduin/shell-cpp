#include "builtins.hpp"

#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <unistd.h>

#include "utils.hpp"

using BuiltinHandler = std::function<void(const std::vector<std::string> &)>;

namespace
{
void handle_echo_command(const std::vector<std::string> &args)
{
    for (size_t i = 1; i < args.size(); i++)
    {
        if (i > 1)
        {
            std::cout << " ";
        }

        std::cout << args[i];
    }

    std::cout << std::endl;
}

void handle_exit_command(const std::vector<std::string> &args)
{
    std::exit(0);
}

void handle_type_command(const std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        return;
    }

    const std::string &target = args[1];

    if (is_builtin_command(target))
    {
        std::cout << target << " is a builtin" << std::endl;
        return;
    }

    std::string executable_path = find_executable(target);

    if (!executable_path.empty())
    {
        std::cout << target << " is " << executable_path << std::endl;
        return;
    }

    std::cout << target << ": not found" << std::endl;
}

void handle_cd_command(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return;
    }

    const std::string& path = args[1];

    if (chdir(path.c_str()) != 0) {
        std::cout << "cd: " << path << ": No such file or directory" << std::endl;
    }
}

void handle_pwd_command(const std::vector<std::string>& args) {
    std::cout << std::filesystem::current_path().string() << std::endl;
}

const std::unordered_map<std::string, BuiltinHandler>& builtin_table() {
    static const std::unordered_map<std::string, BuiltinHandler> table = {
        {"echo", handle_echo_command},
        {"exit", handle_exit_command},
        {"type", handle_type_command},
        {"pwd", handle_pwd_command},
        {"cd", handle_cd_command}
    };
}
} //namespace

bool is_builtin_command(const std::string& command) {
    const auto& table = builtin_table();
    return table.find(command) != table.end();
}

bool execute_builtin_command(const std::vector<std::string>& args) {
    if (args.empty()) {
        return false;
    }

    const std::string& command = args[0];
    const auto& table = builtin_table();

    auto it = table.find(command);

    if (it == table.end()) {
        return false;
    }

    it->second(args);
    return true;
}