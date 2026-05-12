#include "utils.hpp"

#include <cstdlib>
#include <sstream>
#include <unistd.h>

std::vector<std::string> split_input(const std::string &input)
{
    std::vector<std::string> args;
    std::stringstream ss(input);
    std::string arg;

    while (ss >> arg)
    {
        args.push_back(arg);
    }

    return args;
}

std::vector<std::string> split_path(const std::string &path_env)
{
    std::vector<std::string> dirs;
    std::stringstream ss(path_env);
    std::string dir;

    while (std::getline(ss, dir, ':'))
    {
        if (!dir.empty())
        {
            dirs.push_back(dir);
        }
    }

    return dirs;
}

bool is_executable_file(const std::filesystem::path &path)
{
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path) && access(path.c_str(), X_OK) == 0;
}

std::string find_executable(const std::string &command)
{
    const char *path_env = std::getenv("PATH");

    if (path_env == nullptr)
    {
        return "";
    }

    std::vector<std::string> dirs = split_path(path_env);

    for (const std::string &dir : dirs)
    {
        std::filesystem::path full_path = std::filesystem::path(dir) / command;

        if (is_executable_file(full_path))
        {
            return full_path.string();
        }
    }

    return "";
}