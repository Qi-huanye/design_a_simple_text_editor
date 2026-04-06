#include "editorWindow.h"
#include <FL/Fl_Native_File_Chooser.H>
#include <iostream>

EditorWindow::EditorWindow(int w, int h, const char* title) : Fl_Double_Window(w, h, title) {
  textEditor = new Fl_Text_Editor(0, 25, w, h - 25);
  textEditor->buffer(textBuffer);

  menuBar = new Fl_Menu_Bar(0, 0, this->w(), 25);
  menuBar->add("File/Open", 0, Open, this);
  menuBar->add("File/Save", 0, Save, this);

  resizable(textEditor);
  end();
}

void EditorWindow::open(const char* fileName) {
  if (!textBuffer.loadfile(fileName)) {
    currentFileName = fileName;
  }
}

void EditorWindow::save(const char* fileName) {
  if (!textBuffer.savefile(fileName)) {
    currentFileName = fileName;
  }
}

void EditorWindow::save() {
  if (!currentFileName.empty()) {
    textBuffer.savefile(currentFileName.c_str());
  }
}

void EditorWindow::Open(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  Fl_Native_File_Chooser fileChooser;
  fileChooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
  int isSuccess = fileChooser.show();
  if (!(isSuccess)){
    const char* filename = fileChooser.filename();
    self->open(filename);
  } else if (isSuccess == 1){
    return;
  } else {
    fl_alert(fileChooser.errmsg());
  }
}

void EditorWindow::Save(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  self->save();
}