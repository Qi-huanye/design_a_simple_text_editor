#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Widget.H>
#include <string>

class EditorTextWidget;

class EditorWindow : public Fl_Double_Window {
public:
  EditorWindow(int w, int h, const char* title);

private:
  friend class EditorTextWidget;
  void open(const char* fileName);
  void save(const char* fileName);
  void save();
  void saveAs();
  void newFile();
  bool confirmDiscardChange();
  void updateTitle();
  void updateStatusBar();
  bool findMatchFrom(int startPos, const std::string& searchText, int& findPos,
                     bool wrapAround = true) const;
  bool findPreviousMatchFrom(int startPos, const std::string& searchText, int& findPos,
                             bool wrapAround = true) const;
  void selectMatch(int startPos, int length);
  void selectAllText();
  bool goToLineNumber(int lineNumber);
  void deleteCurrentLine();
  void toggleLineNumbers();
  void toggleWordWrap();
  int lineCount() const;
  int currentLineNumber() const;
  int currentColumnNumber() const;
  int replaceAllMatches(const std::string& oldText, const std::string& newText, int& lastMatchPos);
  static void Open(Fl_Widget*, void*);
  static void Save(Fl_Widget*, void*);
  static void SaveAs(Fl_Widget*, void*);
  static void New(Fl_Widget*, void*);
  static void Close(Fl_Widget*, void*);
  static void Changed(int pos, int nInserted, int nDeleted, int nRestyled, const char* deletedText,
                      void* cbArg);
  static void Undo(Fl_Widget*, void*);
  static void Redo(Fl_Widget*, void*);
  static void Cut(Fl_Widget*, void*);
  static void Copy(Fl_Widget*, void*);
  static void Paste(Fl_Widget*, void*);
  static void SelectAll(Fl_Widget*, void*);
  static void GoToLine(Fl_Widget*, void*);
  static void DeleteCurrentLine(Fl_Widget*, void*);
  static void ToggleLineNumbers(Fl_Widget*, void*);
  static void ToggleWordWrap(Fl_Widget*, void*);
  static void Find(Fl_Widget*, void*);
  static void FindNext(Fl_Widget*, void*);
  static void FindPrevious(Fl_Widget*, void*);
  static void Replace(Fl_Widget*, void*);
  static void ReplaceAll(Fl_Widget*, void*);
  Fl_Menu_Bar* menuBar;
  Fl_Box* statusBar;
  Fl_Text_Editor* textEditor;
  Fl_Text_Buffer textBuffer;
  std::string currentFileName;
  std::string lastFindString;
  bool lineNumbersEnabled = true;
  bool wordWrapEnabled = false;
  bool modified = false;
};

#endif
