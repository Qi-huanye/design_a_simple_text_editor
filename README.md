# Simple Text Editor

A lightweight GUI text editor written in C++ with FLTK.

## Branches

- `master`: current FLTK GUI editor
- `cli_text_editor`: preserved command-line editor version

If you want the original CLI editor, switch branches:

```bash
git checkout cli_text_editor
```

## Features

- Create, open, save, and save files as new names
- Open a file directly from the command line
- Undo, redo, cut, copy, paste, and select all
- Find, find next, find previous, replace, and replace all
- Optional match-case search mode
- Go to line, delete current line, and duplicate current line
- Toggle word wrap and line numbers
- Live status bar with file state, cursor line/column, and document statistics

## Requirements

- CMake 3.10 or later
- A C++17 compiler
- FLTK 1.4

## Build

```bash
mkdir build
cd build
cmake ..
make
```

## Run On `master`

Start an empty editor window:

```bash
./build/text_editor
```

Open a file immediately:

```bash
./build/text_editor demo.txt
```

## Menus

- `File`: new, open, save, save as
- `Edit`: undo/redo, clipboard actions, search and replace, line tools
- `View`: line numbers and word wrap

## Notes

- The status bar shows whether the file is saved or modified.
- Search supports forward and backward navigation.
- `Replace All` shows how many matches were replaced.
- The `cli_text_editor` branch keeps the earlier command-line workflow.
