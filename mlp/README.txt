Directory Structure
===================

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

./include/x/
	Header files
	------------
	Libraries header (.h) files.

./src/x/
	Source files
	------------
	Libraries source (.cpp) files.

./tests/x/
	Tests
	-----
	Executable test source files.
	Tests are targeted as xtest.

./vis/app/
	Graphical applications
	----------------------
	Source files for executables with some
	graphical interface.
	These are targeted as xvisapp.

./vis/src/
	Graphical libraries source files
	--------------------------------
	Source files for libraries that interact
    with OpenGL.
    
./vis/include/
	Graphical libraries header files
	--------------------------------
	Header files for libraries that interact
    with OpenGL.

Libraries
=========

A library named x has its source files located at
src/x/ and its header files at include/x/. It can
have a test executable located at tests/x/.
Libraries are targeted as xlib.

Graphical libraries are located in the vis/ directory
and have their source files located at vis/src/x/
and header files located at vis/include/x/.
Graphical libraries are targeted as xvislib.

Each library should contain a README file to
document the code. Documenting in the code is
discouraged.

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
	- All header files must be in a folder
	  with the same name in include/.
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

    ...a graphical library
	----------------------
	- Create a folder in vis/src/.
	- There, add at least one source file
	- All header files must be in a folder
	  with the same name in vis/include/.
	- If it depends on any library, create
	  a CMakeLists.txt file inside and there
	  link them accordingly.

	...a graphical application
	--------------------------
	- Create a folder in vis/app/.
	- There, add at least one source file
	- It will be automatically linked to the
	  graphics libraries. But if there are
	  any other dependencies, create a
	  CMakeLists.txt file inside and there
	  link them accordingly.