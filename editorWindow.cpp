#include "editorWindow.h"
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>
#include <cstdlib>

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
  menuBar->add("Edit/Redo", FL_COMMAND + 'y', Redo, this);
  menuBar->add("Edit/Cut", FL_COMMAND + 'x', Cut, this);
  menuBar->add("Edit/Copy", FL_COMMAND + 'c', Copy, this);
  menuBar->add("Edit/Paste", FL_COMMAND + 'v', Paste, this);
  menuBar->add("Edit/Select All", FL_COMMAND + 'a', SelectAll, this);
  menuBar->add("Edit/Delete Current Line", FL_COMMAND + 'd', DeleteCurrentLine, this);
  menuBar->add("Edit/Go To Line", FL_COMMAND + 'g', GoToLine, this);
  menuBar->add("Edit/Find", FL_COMMAND + 'f', Find, this);
  menuBar->add("Edit/Find Next", 0, FindNext, this);
  menuBar->add("Edit/Replace", 0, Replace, this);
  menuBar->add("Edit/Replace All", 0, ReplaceAll, this);

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

bool EditorWindow::findMatchFrom(int startPos, const std::string& searchText, int& findPos,
                                 bool wrapAround) const {
  if (searchText.empty()) {
    return false;
  }

  int safeStart = startPos;
  if (safeStart < 0) {
    safeStart = 0;
  }

  const int bufferLength = textBuffer.length();
  if (safeStart > bufferLength) {
    safeStart = bufferLength;
  }

  if (textBuffer.search_forward(safeStart, searchText.c_str(), &findPos)) {
    return true;
  }

  if (wrapAround && safeStart > 0) {
    return textBuffer.search_forward(0, searchText.c_str(), &findPos) != 0;
  }

  return false;
}

void EditorWindow::selectMatch(int startPos, int length) {
  textBuffer.select(startPos, startPos + length);
  textEditor->insert_position(startPos + length);
  textEditor->show_insert_position();
  textEditor->take_focus();
}

void EditorWindow::selectAllText() {
  textBuffer.select(0, textBuffer.length());
  textEditor->insert_position(textBuffer.length());
  textEditor->show_insert_position();
  textEditor->take_focus();
}

int EditorWindow::lineCount() const {
  if (textBuffer.length() == 0) {
    return 1;
  }

  return textBuffer.count_lines(0, textBuffer.length()) + 1;
}

int EditorWindow::currentLineNumber() const {
  return textBuffer.count_lines(0, textEditor->insert_position()) + 1;
}

bool EditorWindow::goToLineNumber(int lineNumber) {
  if (lineNumber < 1 || lineNumber > lineCount()) {
    return false;
  }

  int position = 0;
  if (lineNumber > 1) {
    position = textBuffer.skip_lines(0, lineNumber - 1);
  }

  textBuffer.unselect();
  textEditor->insert_position(position);
  textEditor->show_insert_position();
  textEditor->take_focus();
  return true;
}

void EditorWindow::deleteCurrentLine() {
  const int bufferLength = textBuffer.length();
  if (bufferLength == 0) {
    fl_alert("Buffer is empty");
    return;
  }

  const int cursorPos = textEditor->insert_position();
  const int lineStart = textBuffer.line_start(cursorPos);
  const int lineEnd = textBuffer.line_end(cursorPos);

  int deleteStart = lineStart;
  int deleteEnd = lineEnd;

  if (lineEnd < bufferLength) {
    deleteEnd = lineEnd + 1;
  } else if (lineStart > 0) {
    deleteStart = lineStart - 1;
  }

  textBuffer.remove(deleteStart, deleteEnd);
  textBuffer.unselect();
  textEditor->insert_position(deleteStart);
  textEditor->show_insert_position();
  textEditor->take_focus();
}

int EditorWindow::replaceAllMatches(const std::string& oldText, const std::string& newText,
                                    int& lastMatchPos) {
  int startPos = 0;
  int findPos = 0;
  int count = 0;
  lastMatchPos = -1;

  while (findMatchFrom(startPos, oldText, findPos, false)) {
    textBuffer.replace(findPos, findPos + oldText.size(), newText.c_str());
    startPos = findPos + newText.size();
    lastMatchPos = findPos;
    count++;
  }

  return count;
}

void EditorWindow::Undo(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  Fl_Text_Editor::kf_undo(0, self->textEditor);
}

void EditorWindow::Redo(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  Fl_Text_Editor::kf_redo(0, self->textEditor);
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

void EditorWindow::SelectAll(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  self->selectAllText();
}

void EditorWindow::GoToLine(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);

  std::string currentLine = std::to_string(self->currentLineNumber());
  const char* input = fl_input("Go to line:", currentLine.c_str());
  if (input == nullptr || input[0] == '\0') {
    return;
  }

  char* endPtr = nullptr;
  const long lineNumber = std::strtol(input, &endPtr, 10);
  if (endPtr == input || *endPtr != '\0' || lineNumber < 1 || lineNumber > self->lineCount()) {
    std::string message = "Line number must be between 1 and " + std::to_string(self->lineCount()) + ".";
    fl_alert("%s", message.c_str());
    return;
  }

  self->goToLineNumber(static_cast<int>(lineNumber));
}

void EditorWindow::DeleteCurrentLine(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  self->deleteCurrentLine();
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
  if (self->findMatchFrom(self->textEditor->insert_position(), findString, findPos)) {
    self->selectMatch(findPos, findString.size());
  } else {
    fl_alert("Not Found");
  }
}

void EditorWindow::FindNext(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);

  if (self->lastFindString.empty()) {
    fl_alert("No previous search");
    return;
  }

  int findPos;
  if (self->findMatchFrom(self->textEditor->insert_position(), self->lastFindString, findPos)) {
    self->selectMatch(findPos, self->lastFindString.size());
  } else {
    fl_alert("Not Found");
  }
}

void EditorWindow::Replace(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);

  const char* oldInput = fl_input("Find:");
  if (oldInput == nullptr || oldInput[0] == '\0') {
    return;
  }
  std::string oldText = oldInput;

  const char* newInput = fl_input("Replace with:");
  if (newInput == nullptr) {
    return;
  }
  std::string newText = newInput;

  self->lastFindString = oldText;

  int selStart = 0;
  int selEnd = 0;
  self->textBuffer.selection_position(&selStart, &selEnd);

  if (selEnd > selStart) {
    char* selectedText = self->textBuffer.selection_text();
    if (selectedText != nullptr) {
      std::string selected = selectedText;
      std::free(selectedText);

      if (selected == oldText) {
        self->textBuffer.replace(selStart, selEnd, newText.c_str());
        self->selectMatch(selStart, newText.size());
        return;
      }
    }
  }

  int findPos;
  if (self->findMatchFrom(self->textEditor->insert_position(), oldText, findPos)) {
    self->textBuffer.replace(findPos, findPos + oldText.size(), newText.c_str());
    self->selectMatch(findPos, newText.size());
  } else {
    fl_alert("Not Found");
  }
}

void EditorWindow::ReplaceAll(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);

  const char* oldInput = fl_input("Find:");
  if (oldInput == nullptr || oldInput[0] == '\0') {
    return;
  }
  std::string oldText = oldInput;

  const char* newInput = fl_input("Replace with:");
  if (newInput == nullptr) {
    return;
  }
  std::string newText = newInput;

  self->lastFindString = oldText;

  int lastMatchPos = -1;
  const int count = self->replaceAllMatches(oldText, newText, lastMatchPos);

  if (count == 0) {
    fl_alert("Not Found");
  } else {
    if (newText.empty()) {
      self->textBuffer.unselect();
      self->textEditor->insert_position(lastMatchPos);
      self->textEditor->show_insert_position();
      self->textEditor->take_focus();
    } else {
      self->selectMatch(lastMatchPos, newText.size());
    }

    std::string message = "Replaced " + std::to_string(count) + " occurrence(s).";
    fl_alert("%s", message.c_str());
  }
}
