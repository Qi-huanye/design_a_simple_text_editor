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
    std::string command;
    std::string cmd;
    std::cout << "\n> ";
    std::getline(std::cin, command);
    std::istringstream iss(command);

    if (!(iss >> cmd)) {
      continue;
    }

    if (cmd == "help") {
      showHelp();
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
      handleInsert(iss);
    } else if (cmd == "delete") {
      handleDelete(iss);
    } else if (cmd == "edit") {
      handleEdit(iss);
    } else if (cmd == "saveas") {
      handleSaveAs(iss);
    } else if (cmd == "open") {
      handleOpen(iss);
    } else if (cmd == "save") {
      save();
    } else if (cmd == "status") {
      status();
    } else if (cmd == "find") {
      handleFind(iss);
    } else if (cmd == "replace") {
      handleReplace(iss);
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

void textEditor::status() {
  if (currentFileName.empty()) {
    std::cout << "Unnamed File" << std::endl;
  } else {
    std::cout << currentFileName << std::endl;
  }

  if (modified) {
    std::cout << "Unsaved File" << std::endl;
  } else {
    std::cout << "Saved File" << std::endl;
  }

  std::cout << "Total lines: " << file.size() << std::endl;
}

void textEditor::find(const std::string& findString) {
  int lineNum = 1;
  for (const std::string& line : file) {
    if (line.find(findString) != std::string::npos) {
      std::cout << lineNum << ":" << line << std::endl;
    }
    lineNum++;
  }
}

bool textEditor::findFile(const std::vector<std::string>& file, const std::string& findString) {
  bool isFind = false;
  for (const std::string& line : file) {
    if (line.find(findString) != std::string::npos) {
      isFind = true;
      return isFind;
    }
  }
  return isFind;
}

void textEditor::replace(const std::string& oldText, const std::string& newText) {
  for (size_t i = 0; i < file.size(); i++) {
    std::string& line = file[i];
    size_t position = line.find(oldText);
    while (position != std::string::npos) {
      line.replace(position, oldText.length(), newText);
      position = line.find(oldText, position + newText.length());
      modified = true;
    }
  }
}

void textEditor::handleInsert(std::istringstream& iss) {
  std::string context;
  int lineNum;
  if (!(iss >> lineNum)) {
    std::cout << "Please input correct line number" << std::endl;
    return;
  }
  std::getline(iss >> std::ws, context);
  insert(lineNum, context);
}

void textEditor::handleDelete(std::istringstream& iss) {
  int lineNum;
  if (!(iss >> lineNum)) {
    std::cout << "Please input correct line number" << std::endl;
    return;
  }
  deleteLine(lineNum);
}

void textEditor::handleEdit(std::istringstream& iss) {
  std::string context;
  int lineNum;
  if (!(iss >> lineNum)) {
    std::cout << "Please input correct line number" << std::endl;
    return;
  }
  std::getline(iss >> std::ws, context);
  edit(lineNum, context);
}

void textEditor::handleOpen(std::istringstream& iss) {
  std::string fileName;
  if (modified) {
    std::cout << "You have unsaved changes" << std::endl;
    return;
  }
  if (!(iss >> fileName)) {
    std::cout << "Please input a correct filename" << std::endl;
    return;
  }
  open(fileName);
}

void textEditor::handleSaveAs(std::istringstream& iss) {
  std::string fileName;
  if (!(iss >> fileName)) {
    std::cout << "Please input a correct filename" << std::endl;
    return;
  }
  saveas(fileName);
}

void textEditor::handleReplace(std::istringstream& iss) {
  std::string oldText, newText;
  if (!(iss >> oldText)) {
    std::cout << "Please input correct to be replaced string" << std::endl;
    return;
  }

  if (!(iss >> newText)) {
    std::cout << "Please input correct replaced string" << std::endl;
    return;
  }

  if (!(findFile(file, oldText))) {
    std::cout << "Dont find the to be replaced string" << std::endl;
    return;
  }

  replace(oldText, newText);
}

void textEditor::handleFind(std::istringstream& iss) {
  std::string findString;
  if (!(std::getline(iss >> std::ws, findString))) {
    std::cout << "Please input correct string" << std::endl;
    return;
  }
  if (!(findFile(file, findString))) {
    std::cout << "Dont find the string" << std::endl;
    return;
  }
  find(findString);
}

void showHelp() {
  std::cout << "Available commands:" << std::endl;
  std::cout << "basic commands: help, quit, status" << std::endl; 
  std::cout << "file commands: new, open, save, saveas" << std::endl;
  std::cout << "edit commands: show, insert, delete, edit.find, replace" << std::endl;
}