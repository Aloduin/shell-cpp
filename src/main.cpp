#include <iostream>
#include <string>
#include <vector>

#include "builtins.hpp"
#include "executor.hpp"
#include "utils.hpp"

namespace {

void print_prompt() {
    std::cout << "$ ";
    std::cout.flush();
}

void handle_input(const std::string& input) {
    std::vector<std::string> args = split_input(input);

    if (args.empty()) {
        return;
    }

    if (execute_builtin_command(args)) {
        return;
    }

    execute_external_command(args);
}

} // namespace

int main() {
    std::string input;

    while (true) {
        print_prompt();

        if (!std::getline(std::cin, input)) {
            break;
        }

        handle_input(input);
    }

    return 0;
}