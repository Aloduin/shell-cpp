#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>
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
    ParsedCommand cmd = parse_command(input);

    if (cmd.args.empty()) {
        return;
    }

    int saved_stdout = -1;
    int saved_stderr = -1;

    if (cmd.redirect_stdout) {
        saved_stdout = dup(STDOUT_FILENO);

        int fd = open(
            cmd.stdout_file.c_str(),
            O_WRONLY | O_CREAT | O_TRUNC,
            0644
        );

        if (fd == -1) {
            std::cerr << "Failed to open file: "
                      << cmd.stdout_file << std::endl;
            return;
        }

        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    if (cmd.redirect_stderr) {
        saved_stderr = dup(STDERR_FILENO);

        int fd = open(
            cmd.stderr_file.c_str(),
            O_WRONLY | O_CREAT | O_TRUNC,
            0644
        );

        if (fd == -1) {
            std::cerr << "Failed to open file: "
                      << cmd.stderr_file << std::endl;

            if (cmd.redirect_stdout) {
                dup2(saved_stdout, STDOUT_FILENO);
                close(saved_stdout);
            }

            return;
        }

        dup2(fd, STDERR_FILENO);
        close(fd);
    }

    if (!execute_builtin_command(cmd.args)) {
        execute_external_command(cmd.args);
    }

    if (cmd.redirect_stdout) {
        std::cout.flush();
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
    }

    if (cmd.redirect_stderr) {
        std::cerr.flush();
        dup2(saved_stderr, STDERR_FILENO);
        close(saved_stderr);
    }
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