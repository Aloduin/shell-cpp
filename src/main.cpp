#include <iostream>
#include <string>

void isBuiltin(std::string command) {
  if (command == "echo" || command == "exit" || command == "type") {
    std::cout << command << " is a shell builtin" << std::endl;
  }
  else {
    std::cout << command << ": not found" << std::endl;
  }
}

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while (true) {
    // TODO: Uncomment the code below to pass the first stage
    std::cout << "$ ";

    // Captures the user's command in the "command" variable
    std::string command;
    std::getline(std::cin, command);

    if (command == "exit") break;

    if (command.substr(0, 5) == "echo ") {
      std::cout << command.substr(5) << std::endl;
    }
    else if (command.substr(0, 5) == "type ") {
      isBuiltin(command.substr(5));
    }
    else {
      // Prints the {command}: command not found
      std::cout << command << ": command not found" << std::endl;
    }

  }
}
