#include "editorWindow.h"
#include <FL/Fl.H>

int main(){
  EditorWindow win = {800, 800, "Text Editor"};
  win.show();
  return Fl::run();
}