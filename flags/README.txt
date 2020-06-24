Flags
=====

2nd project of the IA course.

Objective
---------

Recognise countries' caracteristics through their flags.

Source
------

Dataset "Flags" from UCI: https://archive.ics.uci.edu/ml/datasets/Flags

How to build the binding "importbindlib" to Python
--------------------------------------------------

0. Make sure there is a flags/ext/pybind11 folder with a bunch of files there.
   If not, you should pull the repository recursively with:
   
   $ git clone --recursive <git repo URL>
   
1. Get yourself a copy of the latest version of CMake

   ps: if you don't know CMake already, it is basically a platform independent
       C/C++ project builder, so to say. It creates Makefiles, VS solutions etc
       all based on these CMakeLists.txt that tell him what and how to build
       libraries and executables. An essential tool for C/C++ developers.

   Windows
   =======
   - Go to https://cmake.org/
   - Click on 'Download'
   - Go for the Installer
   - Install it and don't forget to add it to PATH

   Linux
   =====
   - Install through your favourite package manager (might be outdated)
   - or compile the source code (read https://askubuntu.com/a/865294)

2. On your favourite terminal, open it in this very directory (flags/) and write:

    $ mkdir build
    $ cd build
    $ cmake ..

   If it says "Python libraries not found" or something like that, you may
   try one of the following solutions, then after each one, delete this
   build/CMakeCache.txt file and run the same command again.
   
   - make sure you have the Python debug libraries installed [see footnote 1]
   - specify which Python libraries and headers to work with [see footnote 2]
   
[1] By default, Python doesn't come with all the resources we'll need to
    build Python extension modules. That's why we should download the
    developer version of the language.

    Windows
    =======

    When installing your Python interpreter, don't go for the default
    installation. Instead, go for the 'Customized instalation', and check ALL
    the options. One of them include the libraries.

    Linux
    =====

    If you use python:
    $ sudo apt install python-dev

    If you use python3:
    $ sudo apt install python3-dev

    If you use python3.X:
    $ sudo apt install python3.X-dev

[2] Specify which Python version you'll be using with:
    $ cmake -DPYTHON_EXECUTABLE:FILEPATH=### ..
    Where ### is the path to your Python executable, eg:

    Windows
    =======
    C:/Program Files/Python38/python.exe

    Linux
    =====
    /usr/bin/python38

3. Now, compile everything with:
   $ cmake --build . --config Release
   
4. Done! Now you can import the module by calling "import instance" in flags/py.
