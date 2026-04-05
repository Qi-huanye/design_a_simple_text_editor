#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>

class EditorWindow : public Fl_Double_Window {
public:
  EditorWindow(int w, int h, const char* title);

private:
  Fl_Text_Editor* textEditor;
  Fl_Text_Buffer textBuffer;
};

#endif