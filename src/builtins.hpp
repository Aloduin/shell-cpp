#pragma once


#include <string>
#include <vector>


bool is_builtin_command(const std::string& command);

bool execute_builtin_command(const std::vector<std::string>& args);
