#include "textEditor.h"
#include <fstream>
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
    std::string fileName;
    std::cout << "\n> ";
    std::getline(std::cin, command);
    std::istringstream iss(command);

    if (!(iss >> cmd)) {
      continue;
    }

    if (cmd == "help") {
      std::cout
          << "Available commands: help, quit, show, new, insert, delete, edit, saveas, open, save"
          << std::endl;

    } else if (cmd == "quit") {
      if (modified) {
        std::cout << "You have unsaved changes" << std::endl;
        continue;
      }
      std::cout << "Goodbye!" << std::endl;
      break;

    } else if (cmd == "new") {
      if (modified) {
        std::cout << "You have unsaved changes" << std::endl;
        continue;
      }
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

    } else if (cmd == "saveas") {
      if (!(iss >> fileName)) {
        std::cout << "Please input a correct filename" << std::endl;
        continue;
      }
      saveas(fileName);

    } else if (cmd == "open") {
      if (modified) {
        std::cout << "You have unsaved changes" << std::endl;
        continue;
      }
      if (!(iss >> fileName)) {
        std::cout << "Please input a correct filename" << std::endl;
        continue;
      }
      open(fileName);

    } else if (cmd == "save") {
      save();
    } else {
      std::cout << "Unknown command" << std::endl;
    }
  }
}

void textEditor::newFile() {
  file.clear();
  currentFileName.clear();
  std::cout << "New file created" << std::endl;

  modified = false;
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

  modified = true;
}

void textEditor::deleteLine(const int& lineNum) {
  if (lineNum < 1 || lineNum > file.size()) {
    std::cout << "You can not delete this line" << std::endl;
    return;
  }

  size_t index = lineNum - 1;
  file.erase(file.begin() + index);

  modified = true;
}

void textEditor::edit(const int& lineNum, const std::string& context) {
  if (lineNum < 1 || lineNum > file.size()) {
    std::cout << "You can not edit here" << std::endl;
    return;
  }

  size_t index = lineNum - 1;
  file[index] = context;

  modified = true;
}

void textEditor::saveas(const std::string& fileName) {
  std::ofstream fout(fileName);
  if (!fout) {
    std::cout << "Cannot open file" << std::endl;
    return;
  }
  for (const std::string& line : file) {
    fout << line << std::endl;
  }
  currentFileName = fileName;
  std::cout << "Save to " << fileName << std::endl;

  modified = false;
}

void textEditor::open(const std::string& fileName) {
  std::ifstream fin(fileName);
  if (!fin) {
    std::cout << "Cannot open file" << std::endl;
    return;
  }

  file.clear();

  std::string line;
  while (std::getline(fin, line)) {
    file.push_back(line);
  }
  currentFileName = fileName;
  std::cout << "Opened the " << fileName << std::endl;

  modified = false;
}

void textEditor::save() {
  if (currentFileName.empty()) {
    std::cout << "Please open a file or use saveas to save as a new file" << std::endl;
    return;
  }
  saveas(currentFileName);
}