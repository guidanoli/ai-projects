Directory Structure
===================

./app/
	Applications
	------------
	Executable source files.
	Organized in individual folders.
	Applications are targeted as app*.

./cmake/
	CMake files
	-----------
	Macros and function definitions.

./data/
	Data files
	----------
	Files used for storing data only.

./include/
	Header files
	------------
	Organized in individual folders.

./src/
	Source files
	------------
	Organized in individual folders.

./tests/
	Tests
	-----
	Executable source files.
	Are meant to test libraries.
	Organized in individual folders.
	Tests are targeted as t*.

Libraries
=========

A library named x has its source files located at
src/x/ and its header files at include/x/. It can
have N test cases, all located at tests/x/.
Libraries are targeted as lib*.

CMake
=====

CMake does the magic of globbing to automatically
create targets for each of the subdirectories.
But what CMake doesn't do alone is figure out
dependencies. Therefore, you must include a
CMakeLists.txt in each subfolder and link the
appropriate libraries to your target.