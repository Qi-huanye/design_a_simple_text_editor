#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include <string>
#include <vector>

class textEditor {
public:
  void run();
  void newFile();
  void show();

private:
  std::vector<std::string> file;
};

#endif