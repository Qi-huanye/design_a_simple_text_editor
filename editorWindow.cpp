#include "editorWindow.h"
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>

EditorWindow::EditorWindow(int w, int h, const char* title) : Fl_Double_Window(w, h, title) {
  textEditor = new Fl_Text_Editor(0, 25, w, h - 25 - 24);
  textEditor->buffer(textBuffer);

  statusBar = new Fl_Box(0, h - 24, w, 24);

  menuBar = new Fl_Menu_Bar(0, 0, this->w(), 25);
  menuBar->add("File/Open", 0, Open, this);
  menuBar->add("File/Save", 0, Save, this);
  menuBar->add("File/Save As", 0, SaveAs, this);
  menuBar->add("File/New", 0, New, this);

  textBuffer.add_modify_callback(Changed, this);
  resizable(textEditor);

  end();
  updateTitle();
  updateStatusBar();
  callback(Close, this);
}

void EditorWindow::open(const char* fileName) {
  if (!textBuffer.loadfile(fileName)) {
    currentFileName = fileName;
    modified = false;
    updateTitle();
    updateStatusBar();
  } else {
    fl_alert("Failure to Open");
  }
}

void EditorWindow::save(const char* fileName) {
  if (!textBuffer.savefile(fileName)) {
    currentFileName = fileName;
    modified = false;
    updateTitle();
    updateStatusBar();
  } else {
    fl_alert("Failure to Save");
  }
}

void EditorWindow::save() {
  if (!currentFileName.empty()) {
    save(currentFileName.c_str());
  } else {
    saveAs();
  }
}

void EditorWindow::Open(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  Fl_Native_File_Chooser fileChooser;
  fileChooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
  fileChooser.title("Open File");
  int isSuccess;
  if (self->confirmDiscardChange()) {
    isSuccess = fileChooser.show();
  } else
    return;
  if (!(isSuccess)) {
    const char* filename = fileChooser.filename();
    self->open(filename);
  } else if (isSuccess == 1) {
    return;
  } else {
    fl_alert("%s", fileChooser.errmsg());
  }
}

void EditorWindow::Save(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  self->save();
}

void EditorWindow::SaveAs(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  self->saveAs();
}

void EditorWindow::saveAs() {
  Fl_Native_File_Chooser fileChooser;
  fileChooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
  fileChooser.title("Save File As");
  fileChooser.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM);
  if (!currentFileName.empty())
    fileChooser.preset_file(currentFileName.c_str());
  int isSuccess = fileChooser.show();
  if (!(isSuccess)) {
    save(fileChooser.filename());
  } else if (isSuccess == 1) {
    return;
  } else {
    fl_alert("%s", fileChooser.errmsg());
  }
}

void EditorWindow::New(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  if (self->confirmDiscardChange())
    self->newFile();
}

void EditorWindow::newFile() {
  textBuffer.text("");
  currentFileName.clear();
  modified = false;
  updateTitle();
  updateStatusBar();
}

void EditorWindow::Changed(int, int, int, int, const char*, void* cbArg) {
  EditorWindow* self = static_cast<EditorWindow*>(cbArg);
  if (!self->modified) {
    self->modified = true;
    self->updateTitle();
    self->updateStatusBar();
  }
}

bool EditorWindow::confirmDiscardChange() {
  if (!modified) {
    return true;
  } else {
    int choosen =
        fl_choice("You have unsaved file. Do you want to save?", "Cancel", "Save", "Discard");
    if (choosen == 0) {
      return false;
    } else if (choosen == 1) {
      save();
      return !modified;
    } else {
      return true;
    }
  }
}

void EditorWindow::updateTitle() {
  std::string title;
  if (currentFileName.empty()) {
    title = "Untitled";
  } else {
    title = currentFileName;
  }
  if (modified)
    title = "*" + title;
  this->copy_label(title.c_str());
}

void EditorWindow::Close(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  if (self->confirmDiscardChange()) {
    self->hide();
  }
}

void EditorWindow::updateStatusBar() {
  std::string title;
  if (currentFileName.empty()) {
    title = "Untitled";
  } else {
    title = currentFileName;
  }

  if (modified) {
    title = "*" + title;
  }

  statusBar->copy_label(title.c_str());
}