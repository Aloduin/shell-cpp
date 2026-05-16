#include "utils.hpp"

#include <cctype>
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
    bool escaped = false;

    for (char c : input)
    {
        // 上一个字符是反斜杠
        if (escaped)
        {
            current += c;
            has_token = true;
            escaped = false;
        }

        // 处理反斜杠
        else if (c == '\\' && state != QuoteState::SINGLE)
        {
            escaped = true;
            has_token = true;
        }

        // 单引号
        else if (c == '\'' && state != QuoteState::DOUBLE)
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

        // 双引号
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

        // 空格分隔
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

        // 普通字符
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

ParsedCommand parse_command(const std::string &input)
{
    std::vector<std::string> tokens = split_input(input);

    ParsedCommand cmd;

    for (size_t i = 0; i < tokens.size(); i++)
    {
        const std::string &token = tokens[i];

        if ((token == ">" || token == "1>") && i + 1 < tokens.size())
        {
            cmd.redirect_stdout = true;
            cmd.stdout_file = tokens[i + 1];
            i++;
        }
        else if (token == "2>" && i + 1 < tokens.size())
        {
            cmd.redirect_stderr = true;
            cmd.stderr_file = tokens[i + 1];
            i++;
        }
        else if (token.size() > 1 && token[0] == '>')
        {
            cmd.redirect_stdout = true;
            cmd.stdout_file = token.substr(1);
        }
        else if (token.size() > 2 && token.rfind("1>", 0) == 0)
        {
            cmd.redirect_stdout = true;
            cmd.stdout_file = token.substr(2);
        }
        else if (token.size() > 2 && token.rfind("2>", 0) == 0)
        {
            cmd.redirect_stderr = true;
            cmd.stderr_file = token.substr(2);
        }
        else
        {
            cmd.args.push_back(token);
        }
    }

    return cmd;
}