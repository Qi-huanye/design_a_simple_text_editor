#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <string>
#include <FL/Fl_Widget.H>


class EditorWindow : public Fl_Double_Window {
public:
  EditorWindow(int w, int h, const char* title);

private:
  void open(const char* fileName);
  void save(const char* fileName);
  void save();
  void saveAs();
  static void Open(Fl_Widget*, void*);
  static void Save(Fl_Widget*, void*);
  static void SaveAs(Fl_Widget*, void*);
  Fl_Menu_Bar* menuBar;
  Fl_Text_Editor* textEditor;
  Fl_Text_Buffer textBuffer;
  std::string currentFileName;
};

#endif