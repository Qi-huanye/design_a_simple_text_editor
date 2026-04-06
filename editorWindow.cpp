#include "editorWindow.h"
#include <FL/Enumerations.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <cstdlib>

namespace {
constexpr int kMenuHeight = 38;
constexpr int kStatusHeight = 48;
constexpr int kOuterPadding = 12;
constexpr int kInnerGap = 8;

Fl_Color makeColor(unsigned char r, unsigned char g, unsigned char b) {
  return fl_rgb_color(r, g, b);
}

const Fl_Color kWindowBg = makeColor(255, 243, 248);
const Fl_Color kMenuBg = makeColor(255, 219, 232);
const Fl_Color kMenuSelection = makeColor(245, 171, 198);
const Fl_Color kMenuText = makeColor(112, 41, 71);
const Fl_Color kEditorBg = makeColor(255, 250, 252);
const Fl_Color kEditorText = makeColor(82, 34, 53);
const Fl_Color kEditorBorder = makeColor(236, 190, 208);
const Fl_Color kCursorColor = makeColor(194, 66, 118);
const Fl_Color kSelectionColor = makeColor(255, 196, 220);
const Fl_Color kLineNumberBg = makeColor(252, 228, 238);
const Fl_Color kLineNumberText = makeColor(145, 79, 103);
const Fl_Color kStatusStripBg = makeColor(255, 232, 241);
const Fl_Color kStatusFileBg = makeColor(255, 209, 225);
const Fl_Color kStatusFileDirtyBg = makeColor(234, 120, 162);
const Fl_Color kStatusCursorBg = makeColor(255, 242, 247);
const Fl_Color kStatusStatsBg = makeColor(250, 227, 236);
const Fl_Color kStatusModeBg = makeColor(247, 216, 230);
} // namespace

class EditorTextWidget : public Fl_Text_Editor {
public:
  EditorTextWidget(int x, int y, int w, int h, EditorWindow* owner)
      : Fl_Text_Editor(x, y, w, h), owner(owner) {}

  int handle(int event) override {
    const int handled = Fl_Text_Editor::handle(event);

    switch (event) {
    case FL_PUSH:
    case FL_DRAG:
    case FL_RELEASE:
    case FL_KEYDOWN:
    case FL_KEYUP:
    case FL_PASTE:
    case FL_FOCUS:
    case FL_UNFOCUS:
      owner->updateStatusBar();
      break;
    default:
      break;
    }

    return handled;
  }

private:
  EditorWindow* owner;
};

EditorWindow::EditorWindow(int w, int h, const char* title) : Fl_Double_Window(w, h, title) {
  color(kWindowBg);

  textEditor = new EditorTextWidget(0, 0, w, h, this);
  textEditor->buffer(textBuffer);

  statusBar = new Fl_Box(0, 0, w, kStatusHeight);
  statusBar->box(FL_FLAT_BOX);
  statusBar->color(kStatusStripBg);

  statusFileBox = new Fl_Box(0, 0, 0, 0);
  statusCursorBox = new Fl_Box(0, 0, 0, 0);
  statusStatsBox = new Fl_Box(0, 0, 0, 0);
  statusModeBox = new Fl_Box(0, 0, 0, 0);

  menuBar = new Fl_Menu_Bar(0, 0, this->w(), kMenuHeight);
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
  menuBar->add("Edit/Duplicate Current Line", 0, DuplicateCurrentLine, this);
  menuBar->add("Edit/Go To Line", FL_COMMAND + 'g', GoToLine, this);
  menuBar->add("Edit/Match Case", 0, ToggleMatchCase, this, FL_MENU_TOGGLE);
  menuBar->add("Edit/Find", FL_COMMAND + 'f', Find, this);
  menuBar->add("Edit/Find Next", 0, FindNext, this);
  menuBar->add("Edit/Find Previous", 0, FindPrevious, this);
  menuBar->add("Edit/Replace", 0, Replace, this);
  menuBar->add("Edit/Replace All", 0, ReplaceAll, this);
  menuBar->add("View/Line Numbers", 0, ToggleLineNumbers, this, FL_MENU_TOGGLE | FL_MENU_VALUE);
  menuBar->add("View/Word Wrap", 0, ToggleWordWrap, this, FL_MENU_TOGGLE);

  textBuffer.add_modify_callback(Changed, this);
  textEditor->linenumber_width(28);
  resizable(textEditor);
  applyPinkTheme();
  layoutWidgets(w, h);

  end();
  updateTitle();
  updateStatusBar();
  callback(Close, this);
}

void EditorWindow::openFile(const char* fileName) { open(fileName); }

void EditorWindow::resize(int x, int y, int w, int h) {
  Fl_Double_Window::resize(x, y, w, h);
  layoutWidgets(w, h);
}

void EditorWindow::applyPinkTheme() {
  menuBar->box(FL_FLAT_BOX);
  menuBar->down_box(FL_FLAT_BOX);
  menuBar->color(kMenuBg);
  menuBar->selection_color(kMenuSelection);
  menuBar->textcolor(kMenuText);
  menuBar->textfont(FL_HELVETICA_BOLD);
  menuBar->textsize(15);

  textEditor->box(FL_BORDER_BOX);
  textEditor->color(kEditorBg, kSelectionColor);
  textEditor->selection_color(kSelectionColor);
  textEditor->textfont(FL_COURIER);
  textEditor->textsize(15);
  textEditor->textcolor(kEditorText);
  textEditor->cursor_color(kCursorColor);
  textEditor->linenumber_bgcolor(kLineNumberBg);
  textEditor->linenumber_fgcolor(kLineNumberText);
  textEditor->linenumber_font(FL_HELVETICA_BOLD);
  textEditor->linenumber_size(13);
  textEditor->labelcolor(kEditorBorder);

  Fl_Box* statusBoxes[] = {statusFileBox, statusCursorBox, statusStatsBox, statusModeBox};
  for (Fl_Box* box : statusBoxes) {
    box->box(FL_RFLAT_BOX);
    box->labelfont(FL_HELVETICA_BOLD);
    box->labelsize(12);
    box->labelcolor(kMenuText);
    box->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
  }

  statusFileBox->color(kStatusFileBg);
  statusCursorBox->color(kStatusCursorBg);
  statusStatsBox->color(kStatusStatsBg);
  statusModeBox->color(kStatusModeBg);
}

void EditorWindow::layoutWidgets(int width, int height) {
  menuBar->resize(0, 0, width, kMenuHeight);

  const int statusY = height - kStatusHeight;
  statusBar->resize(0, statusY, width, kStatusHeight);

  const int editorX = kOuterPadding;
  const int editorY = kMenuHeight + kInnerGap;
  const int editorWidth = width - 2 * kOuterPadding;
  const int editorHeight = statusY - editorY - kInnerGap;
  textEditor->resize(editorX, editorY, editorWidth > 0 ? editorWidth : 0,
                     editorHeight > 0 ? editorHeight : 0);

  const int segmentY = statusY + 8;
  const int segmentHeight = kStatusHeight - 16;
  const int availableWidth = width - 2 * kOuterPadding - 3 * kInnerGap;
  const int safeWidth = availableWidth > 0 ? availableWidth : 0;
  const int fileWidth = safeWidth * 34 / 100;
  const int cursorWidth = safeWidth * 16 / 100;
  const int statsWidth = safeWidth * 14 / 100;
  const int modeWidth = safeWidth - fileWidth - cursorWidth - statsWidth;

  int segmentX = kOuterPadding;
  statusFileBox->resize(segmentX, segmentY, fileWidth, segmentHeight);
  segmentX += fileWidth + kInnerGap;
  statusCursorBox->resize(segmentX, segmentY, cursorWidth, segmentHeight);
  segmentX += cursorWidth + kInnerGap;
  statusStatsBox->resize(segmentX, segmentY, statsWidth, segmentHeight);
  segmentX += statsWidth + kInnerGap;
  statusModeBox->resize(segmentX, segmentY, modeWidth > 0 ? modeWidth : 0, segmentHeight);
}

std::string EditorWindow::displayFileName() const {
  if (currentFileName.empty()) {
    return "Untitled";
  }

  const std::size_t slashPos = currentFileName.find_last_of("/\\");
  if (slashPos == std::string::npos) {
    return currentFileName;
  }

  return currentFileName.substr(slashPos + 1);
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
  std::string title = displayFileName();
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
  const bool compactFile = statusFileBox->w() < 240;
  const bool compactCursor = statusCursorBox->w() < 150;
  const bool compactStats = statusStatsBox->w() < 140;
  const bool compactMode = statusModeBox->w() < 320;

  std::string fileLabel = modified ? "Edited" : "Saved";
  if (!compactFile) {
    fileLabel += "  " + displayFileName();
  }

  std::string cursorLabel;
  if (compactCursor) {
    cursorLabel = "L" + std::to_string(currentLineNumber()) + " C" +
                  std::to_string(currentColumnNumber());
  } else {
    cursorLabel = "Ln " + std::to_string(currentLineNumber()) + "  Col " +
                  std::to_string(currentColumnNumber());
  }

  std::string statsLabel;
  if (compactStats) {
    statsLabel = std::to_string(lineCount()) + "L  " + std::to_string(textBuffer.length()) + "C";
  } else {
    statsLabel = std::to_string(lineCount()) + " lines  " + std::to_string(textBuffer.length()) +
                 " chars";
  }

  std::string modeLabel;
  if (compactMode) {
    modeLabel = lineNumbersEnabled ? "Nums On" : "Nums Off";
    modeLabel += wordWrapEnabled ? " | Wrap" : " | NoWrap";
    modeLabel += matchCaseEnabled ? " | Case" : " | NoCase";
  } else {
    modeLabel = lineNumbersEnabled ? "Line Nums On" : "Line Nums Off";
    modeLabel += wordWrapEnabled ? "  |  Wrap On" : "  |  Wrap Off";
    modeLabel += matchCaseEnabled ? "  |  Match Case" : "  |  Ignore Case";
  }

  statusFileBox->color(modified ? kStatusFileDirtyBg : kStatusFileBg);
  statusFileBox->labelcolor(modified ? FL_WHITE : kMenuText);
  statusFileBox->copy_label(fileLabel.c_str());
  statusCursorBox->copy_label(cursorLabel.c_str());
  statusStatsBox->copy_label(statsLabel.c_str());
  statusModeBox->copy_label(modeLabel.c_str());
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

  if (textBuffer.search_forward(safeStart, searchText.c_str(), &findPos, matchCaseEnabled)) {
    return true;
  }

  if (wrapAround && safeStart > 0) {
    return textBuffer.search_forward(0, searchText.c_str(), &findPos, matchCaseEnabled) != 0;
  }

  return false;
}

bool EditorWindow::findPreviousMatchFrom(int startPos, const std::string& searchText, int& findPos,
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

  if (textBuffer.search_backward(safeStart, searchText.c_str(), &findPos, matchCaseEnabled)) {
    return true;
  }

  if (wrapAround && safeStart < bufferLength) {
    return textBuffer.search_backward(bufferLength, searchText.c_str(), &findPos,
                                      matchCaseEnabled) != 0;
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

int EditorWindow::currentColumnNumber() const {
  const int cursorPos = textEditor->insert_position();
  const int lineStart = textBuffer.line_start(cursorPos);
  return cursorPos - lineStart + 1;
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

void EditorWindow::duplicateCurrentLine() {
  const int bufferLength = textBuffer.length();
  if (bufferLength == 0) {
    fl_alert("Buffer is empty");
    return;
  }

  const int cursorPos = textEditor->insert_position();
  const int lineStart = textBuffer.line_start(cursorPos);
  const int lineEnd = textBuffer.line_end(cursorPos);
  char* lineTextPtr = textBuffer.text_range(lineStart, lineEnd);
  if (lineTextPtr == nullptr) {
    fl_alert("Failed to duplicate line");
    return;
  }

  std::string lineText = lineTextPtr;
  std::free(lineTextPtr);

  const bool hasTrailingNewline = lineEnd < bufferLength;
  std::string insertedText = lineText;
  if (hasTrailingNewline) {
    insertedText += "\n";
    textBuffer.insert(lineEnd + 1, insertedText.c_str());
    selectMatch(lineEnd + 1, lineText.size());
  } else {
    insertedText.insert(0, "\n");
    textBuffer.insert(lineEnd, insertedText.c_str());
    selectMatch(lineEnd + 1, lineText.size());
  }
}

void EditorWindow::toggleWordWrap() {
  wordWrapEnabled = !wordWrapEnabled;
  if (wordWrapEnabled) {
    textEditor->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0);
  } else {
    textEditor->wrap_mode(Fl_Text_Display::WRAP_NONE, 0);
  }

  updateStatusBar();
  textEditor->redraw();
}

void EditorWindow::toggleLineNumbers() {
  lineNumbersEnabled = !lineNumbersEnabled;
  if (lineNumbersEnabled) {
    textEditor->linenumber_width(48);
  } else {
    textEditor->linenumber_width(0);
  }

  updateStatusBar();
  textEditor->redraw();
}

void EditorWindow::toggleMatchCase() {
  matchCaseEnabled = !matchCaseEnabled;
  updateStatusBar();
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
    std::string message =
        "Line number must be between 1 and " + std::to_string(self->lineCount()) + ".";
    fl_alert("%s", message.c_str());
    return;
  }

  self->goToLineNumber(static_cast<int>(lineNumber));
}

void EditorWindow::DeleteCurrentLine(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  self->deleteCurrentLine();
}

void EditorWindow::DuplicateCurrentLine(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  self->duplicateCurrentLine();
}

void EditorWindow::ToggleWordWrap(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  self->toggleWordWrap();
}

void EditorWindow::ToggleLineNumbers(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  self->toggleLineNumbers();
}

void EditorWindow::ToggleMatchCase(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);
  self->toggleMatchCase();
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

void EditorWindow::FindPrevious(Fl_Widget*, void* data) {
  EditorWindow* self = static_cast<EditorWindow*>(data);

  if (self->lastFindString.empty()) {
    fl_alert("No previous search");
    return;
  }

  const int startPos =
      self->textEditor->insert_position() - static_cast<int>(self->lastFindString.size()) - 1;
  int findPos;
  if (self->findPreviousMatchFrom(startPos, self->lastFindString, findPos)) {
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
