#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <FL/Fl_Double_Window.H>

class EditorWindow : public Fl_Double_Window {
public:
  EditorWindow(int w, int h, const char* title);
};

#endif