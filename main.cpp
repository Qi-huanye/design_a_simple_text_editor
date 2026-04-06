#include "editorWindow.h"
#include <FL/Fl.H>

int main(int argc, char** argv) {
  EditorWindow win = {800, 800, "Text Editor"};
  if (argc > 1) {
    win.openFile(argv[1]);
  }
  win.show();
  return Fl::run();
}
