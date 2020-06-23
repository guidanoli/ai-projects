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
   
1. Get yourself a copy of the latest version of CMake @ https://cmake.org/

   ps: if you don't know CMake already, it is basically a platform independent
       C/C++ project builder, so to say. It creates Makefiles, VS solutions etc
       all based on these CMakeLists.txt that tell him what and how to build
       libraries and executables. An essential tool for C/C++ developers.

   == NOTE ==================================================
   When installing CMake, add the CMake binary folder to your PATH
   environment variable (On Windows). I don't know how CMake is
   installed on POSIX-like systems.
   ==========================================================

2. On your favourite terminal, open it in this very directory (flags/) and write:

    $ mkdir build
    $ cd build
    $ cmake ..

   You basically created a directory called 'build' and on it ran 'cmake ..'
   It should detect the default project builder of your system, but if you'd
   like to change it, use the -G flag accompained by the software name.
   
   == NOTE ==================================================
   If you don't know what generator to use, run:
   $ cmake --help
   which will list at the very end the exact strings it accepts.
   ==========================================================
   
   If it says "Python libraries not found" or something like that, you may
   try one of the following solutions, then run the same command again:
   
   - make sure you have the Python debug libraries installed [see footnote 1]
   - delete build/CMakeCache.txt
   - specify some paths for CMake [see footnote 2]
   
[1] On Windows, when installing your Python interpreter, don't go for the default
    installation. Instead, go for the 'Customized instalation', and check ALL
    the options. One of them include the libraries.
    On Linux, install using your favourite package manager the developers
    Python package, eg.:
    $ yum install python-devel
    $ apt-get install python-dev

[2] Run on your favourite terminal:
    $ cmake .. \
       -DPYTHON_INCLUDE_DIR=$(python -c "from distutils.sysconfig import get_python_inc; print(get_python_inc())")  \
       -DPYTHON_LIBRARY=$(python -c "import distutils.sysconfig as sysconfig; print(sysconfig.get_config_var('LIBDIR'))")

3. Now, compile the library either using the generator you chose or by running on the build directory:

   $ cmake --build . --config Release
   
   or
   
   $ cmake --build . --config Debug
   
   (Be aware that there can be some linking problems with Python libraries when on Debug mode, and that
    the library file must have a '_d' suffix so that 'python_d.exe' finds it... So I normally stick
    with Release configuration)

4. Done! Now you can import the module by calling "import instance" in flags/py.