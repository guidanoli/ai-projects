Directory Structure
===================

./bind/x/
    Binding libraries
    -----------------
    Binded C++ Libraries
    Bindings are targeted as xbindlib.

./app/x/
	Applications
	------------
	Executable source files.
	Applications are targeted as xapp.

./cmake/
	CMake files
	-----------
	Macros and function definitions.

./data/
	Data files
	----------
	Files used for storing data only.
	Its path is assigned to the macro DATAPATH.

./src/x/
	Libraries files
	---------------
	Libraries source (.cpp, .h) files.

./tests/x/
	Tests
	-----
	Executable test source files.
	Tests are targeted as xtest.

Libraries
=========

A library named x has its source files and header
files located at src/x/. It can have a test executable
located at tests/x/. Libraries are targeted as xlib.

CMake
=====

CMake does the magic of globbing to automatically
create targets for each of the subdirectories.
But what CMake doesn't do alone is figure out
dependencies. For that purpose, you can include
a CMakeLists.txt in a subfolder and link the
appropriate libraries to your target.

Adding...
=========

	...a library
	------------
	- Create a folder in src/.
	- There, add at least one source file
	- All test source files must be in a
	  folder with the same name in tests/.
	- If it depends on any library, create
	  a CMakeLists.txt file inside and there
	  link them accordingly.

	...an application
	-----------------
	- Create a folder in app/.
	- There, add at least one source file
	- If it depends on any library, create
	  a CMakeLists.txt file inside and there
	  link them accordingly.
	
	...a test
	---------
	- Create a folder in tests/ named after
	  the library you want to test.
	- There, add at least one source file.
	- It will be automatically linked to the
	  library being test. But if there are
	  any other dependencies, create a
	  CMakeLists.txt file inside and there
	  link them accordingly.

Building...
===========

First, download a copy of CMake at cmake.org.
Now, either launch the GUI program and setup
the build system accordingly, or, more easily,
open your preffered terminal in this folder and
type the following commands:

$ mkdir build
$ cd build
$ cmake ..

Now, you can either use the built project (Makefile, VS solution...)
or build it yourself through CMake like so (still in the build dir.):

$ cmake --build .

You can also specify which type of build you're planning to run:

$ cmake --build . -C Debug

or...

$ cmake --build . -C Release