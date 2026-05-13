#include "utils.hpp"

#include <cstdlib>
#include <sstream>
#include <unistd.h>

std::vector<std::string> split_input(const std::string& input)
{
    std::vector<std::string> args;
    std::string current;

    enum class QuoteState
    {
        NONE,
        SINGLE,
        DOUBLE
    };

    QuoteState state = QuoteState::NONE;

    bool has_token = false;

    for (char c : input)
    {
        if (c == '\'' && state != QuoteState::DOUBLE)
        {
            if (state == QuoteState::SINGLE)
            {
                state = QuoteState::NONE;
            }
            else
            {
                state = QuoteState::SINGLE;
            }

            has_token = true;
        }
        else if (c == '"' && state != QuoteState::SINGLE)
        {
            if (state == QuoteState::DOUBLE)
            {
                state = QuoteState::NONE;
            }
            else
            {
                state = QuoteState::DOUBLE;
            }

            has_token = true;
        }
        else if (
            std::isspace(static_cast<unsigned char>(c))
            && state == QuoteState::NONE
        )
        {
            if (has_token)
            {
                args.push_back(current);
                current.clear();
                has_token = false;
            }
        }
        else
        {
            current += c;
            has_token = true;
        }
    }

    if (has_token)
    {
        args.push_back(current);
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