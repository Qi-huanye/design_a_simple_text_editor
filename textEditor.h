#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include <string>
#include <vector>

class textEditor {
public:
  void run();
  void newFile();
  void show();
  void insert(const int& lineNum, const std::string& context);
  void deleteLine(const int& lineNum);
  void edit(const int& lineNum, const std::string& context);
  void saveas(const std::string& fileName);
  void save();
  void open(const std::string& fileName);

private:
  std::vector<std::string> file;
  std::string currentFileName = "";
  bool modified = false;
};

#endif