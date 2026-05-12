#include "executor.hpp"

#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "utils.hpp"

namespace
{

    std::vector<char *> make_argv(std::vector<std::string> &args)
    {
        std::vector<char *> argv;

        for (std::string &arg : args)
        {
            argv.push_back(arg.data());
        }

        argv.push_back(nullptr);

        return argv;
    }

} // namespace

void execute_external_command(std::vector<std::string> args)
{
    if (args.empty())
    {
        return;
    }

    const std::string &command = args[0];

    std::string executable_path = find_executable(command);

    if (executable_path.empty())
    {
        std::cout << command << ": command not found" << std::endl;
        return;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        std::cerr << "fork failed" << std::endl;
        return;
    }

    if (pid == 0)
    {
        std::vector<char *> argv = make_argv(args);

        execv(executable_path.c_str(), argv.data());

        std::cerr << command << ": execution failed" << std::endl;
        _exit(1);
    }

    int status = 0;

    if (waitpid(pid, &status, 0) == -1)
    {
        std::cerr << "waitpid failed" << std::endl;
    }
}