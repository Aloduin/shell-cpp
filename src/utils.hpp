#pragma once

#include <filesystem>
#include <string>
#include <vector>

std::vector<std::string> split_path(const std::string &path_env);

std::vector<std::string> split_input(const std::string &input);

bool is_executable_file(const std::filesystem::path &path);

std::string find_executable(const std::string &command);
