#include "textEditor.h"
#include <iostream>
#include <string>

void textEditor::run(){
  std::cout << "Simple Text Editor" << std::endl;
  std::cout << "Type help to see commands." << std::endl;
  std::string command;

  while (true){
    std::cout << "\n> ";
    std::getline(std::cin, command);

    if (command == "help"){
      std::cout << "Available commands: help, quit" << std::endl;
    } else if (command == "quit"){
      std::cout << "Goodbye!" << std::endl;
      break;
    } else if (command.empty()){
      continue;
    } else {
      std::cout <<  "Unknown command" << std::endl;
    }
  }
}