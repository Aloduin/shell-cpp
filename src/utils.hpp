#pragma once

#include <filesystem>
#include <string>
#include <vector>

struct ParsedCommand
{
    std::vector<std::string> args;

    std::string stdout_file;
    bool redirect_stdout = false;
    bool append_stdout = false;

    std::string stderr_file;
    bool redirect_stderr = false;
};

std::vector<std::string> split_path(const std::string &path_env);

std::vector<std::string> split_input(const std::string &input);

ParsedCommand parse_command(const std::string &input);

bool is_executable_file(const std::filesystem::path &path);

std::string find_executable(const std::string &command);