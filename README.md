# Simple Text Editor

A simple command-line text editor written in C++.

## Features

- Create a new file
- Open an existing file
- Show file contents with line numbers
- Insert, delete, and edit lines
- Save and save as
- Search and replace text
- Show editor status

## Requirements

- CMake 3.10 or later
- A C++17 compiler

## Build

```bash
mkdir build
cd build
cmake ..
make
```

## Run

```bash
./text_editor
```

## Commands

- `help`: show available commands
- `new`: create a new file
- `open <filename>`: open a file
- `show`: display file contents
- `insert <line> <text>`: insert text at a line
- `delete <line>`: delete a line
- `edit <line> <text>`: edit a line
- `save`: save the current file
- `saveas <filename>`: save as a new file
- `find <text>`: find text in the current file
- `replace <old> <new>`: replace text in the current file
- `status`: show editor status
- `quit`: exit the editor

## Example

```bash
new
insert 1 hello
insert 2 world
show
saveas demo.txt
quit
```
