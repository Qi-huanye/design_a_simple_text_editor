#include "editorWindow.h"

EditorWindow::EditorWindow(int w, int h, const char* title) : Fl_Double_Window(w, h, title){
  textEditor = new Fl_Text_Editor(0, 0, w, h);
  textEditor->buffer(textBuffer);
  resizable(textEditor);
  end();
};