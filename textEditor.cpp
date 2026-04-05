#include "textEditor.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

void textEditor::run() {
  std::cout << "Simple Text Editor" << std::endl;
  std::cout << "Type help to see commands." << std::endl;

  while (true) {
    int lineNum;
    std::string command;
    std::string cmd;
    std::string context;
    std::cout << "\n> ";
    std::getline(std::cin, command);
    std::istringstream iss(command);
    if (!(iss >> cmd)) {
      continue;
    }

    if (cmd == "help") {
      std::cout << "Available commands: help, quit, show, new, insert, delete, edit" << std::endl;
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
      if (!(iss >> lineNum)) {
        std::cout << "Please input correct line number" << std::endl;
        continue;
      }
      std::getline(iss >> std::ws, context);
      insert(lineNum, context);
    } else if (cmd == "delete") {
      if (!(iss >> lineNum)) {
        std::cout << "Please input correct line number" << std::endl;
        continue;
      }

      deleteLine(lineNum);
    } else if (cmd == "edit") {
      if (!(iss >> lineNum)) {
        std::cout << "Please input correct line number" << std::endl;
        continue;
      }
      std::getline(iss >> std::ws, context);
      edit(lineNum, context);
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

void textEditor::deleteLine(const int& lineNum) {
  if (lineNum < 1 || lineNum > file.size()) {
    std::cout << "You can not delete this line" << std::endl;
    return;
  }

  size_t index = lineNum - 1;
  file.erase(file.begin() + index);
}

void textEditor::edit(const int& lineNum, const std::string& context) {
  if (lineNum < 1 || lineNum > file.size()) {
    std::cout << "You can not edit here" << std::endl;
    return;
  }

  size_t index = lineNum - 1;
  file[index] = context;
}