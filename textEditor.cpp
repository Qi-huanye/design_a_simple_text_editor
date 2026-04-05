#include "textEditor.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void textEditor::run() {
  std::cout << "Simple Text Editor" << std::endl;
  std::cout << "Type help to see commands." << std::endl;
  std::string command;
  int lineNum;
  std::string cmd;
  std::string context;

  while (true) {
    std::cout << "\n> ";
    std::getline(std::cin, command);
    std::istringstream iss(command);
    iss >> cmd;

    if (cmd == "help") {
      std::cout << "Available commands: help, quit, show, new, insert" << std::endl;
    } else if (cmd == "quit") {
      std::cout << "Goodbye!" << std::endl;
      break;
    } else if (cmd.empty()) {
      continue;
    } else if (cmd == "new") {
      newFile();
    } else if (cmd == "show") {
      show();
    } else if (cmd == "insert") {
      if(!(iss >> lineNum)){
        std::cout << "Please input correct line number" << std::endl;
        continue;
      }
      std::getline(iss >> std::ws, context);
      insert(lineNum, context);
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

void textEditor::insert(const int& lineNum, const std::string& context) {
  if (lineNum < 1 || lineNum > file.size() + 1) {
    std::cout << "You can not insert here" << std::endl;
    return;
  }

  size_t index = lineNum - 1;
  file.insert(file.begin() + index, context);
}