#include "editorWindow.h"
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>

EditorWindow::EditorWindow(int w, int h, const char* title) : Fl_Double_Window(w, h, title) {
  textEditor = new Fl_Text_Editor(0, 25, w, h - 25 - 24);
  textEditor->buffer(textBuffer);

  statusBar = new Fl_Box(0, h - 24, w, 24);
  statusBar->box(FL_THIN_UP_BOX);
  statusBar->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  statusBar->labelsize(12);

  menuBar = new Fl_Menu_Bar(0, 0, this->w(), 25);
  menuBar->add("File/Open", FL_COMMAND + 'o', Open, this);
  menuBar->add("File/Save", FL_COMMAND + 's', Save, this);
  menuBar->add("File/Save As", 0, SaveAs, this);
  menuBar->add("File/New", FL_COMMAND + 'n', New, this);
  menuBar->add("Edit/Undo", FL_COMMAND + 'z', Undo, this);
  menuBar->add("Edit/Cut", FL_COMMAND + 'x', Cut, this);
  menuBar->add("Edit/Copy", FL_COMMAND + 'c', Copy, this);
  menuBar->add("Edit/Paste", FL_COMMAND + 'v', Paste, this);
  menuBar->add("Edit/Find", FL_COMMAND + 'f', Find, this);
  menuBar->add("Edit/Find Next", 0, FindNext, this);

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
    title = "Modified | " + title;
  } else {
    title = "Saved | " + title;
  }

  statusBar->copy_label(title.c_str());
}

void EditorWindow::Undo(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  Fl_Text_Editor::kf_undo(0, self->textEditor);
}

void EditorWindow::Cut(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  Fl_Text_Editor::kf_cut(0, self->textEditor);
}

void EditorWindow::Copy(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  Fl_Text_Editor::kf_copy(0, self->textEditor);
}

void EditorWindow::Paste(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  Fl_Text_Editor::kf_paste(0, self->textEditor);
}

void EditorWindow::Find(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  const char* input = fl_input("Find:");
  if (input == nullptr || input[0] == '\0') {
    return;
  }
  std::string findString = input;
  self->lastFindString = findString;
  int findPos;
  if (self->textBuffer.search_forward(self->textEditor->insert_position(), findString.c_str(),
                                      &findPos)) {
    self->textBuffer.select(findPos, findPos + findString.size());
    self->textEditor->insert_position(findPos + findString.size());
    self->textEditor->show_insert_position();
    self->textEditor->take_focus();
  } else {
    fl_alert("Not Found");
  }
}

void EditorWindow::FindNext(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  if (self->lastFindString.empty()) {
    fl_alert("No previous search");
  } else {
    int findPos;
    if (self->textBuffer.search_forward(self->textEditor->insert_position(),
                                        self->lastFindString.c_str(), &findPos)) {
      self->textBuffer.select(findPos, findPos + self->lastFindString.size());
      self->textEditor->insert_position(findPos + self->lastFindString.size());
      self->textEditor->show_insert_position();
      self->textEditor->take_focus();
    } else {
      fl_alert("Not Found");
    }
  }
}