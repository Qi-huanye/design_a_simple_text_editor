#include "textEditor.h"
#include <iostream>
#include <string>
#include <vector>

void textEditor::run() {
  std::cout << "Simple Text Editor" << std::endl;
  std::cout << "Type help to see commands." << std::endl;
  std::string command;

  while (true) {
    std::cout << "\n> ";
    std::getline(std::cin, command);

    if (command == "help") {
      std::cout << "Available commands: help, quit, show, new" << std::endl;
    } else if (command == "quit") {
      std::cout << "Goodbye!" << std::endl;
      break;
    } else if (command.empty()) {
      continue;
    } else if (command == "new") {
      newFile();
    } else if (command == "show") {
      show();
    } else {
      std::cout << "Unknown command" << std::endl;
    }
  }
}

void textEditor::newFile() {
  file.clear();
  std::cout << "New file created" << std::endl;
}

void textEditor::show() {
  if (file.empty()) {
    std::cout << "The file is empty" << std::endl;
    return;
  }

  for (size_t i = 0; i < file.size(); i++) {
    std::cout << i + 1 << ":" << file[i] << std::endl;
  }
}