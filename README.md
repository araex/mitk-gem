# Description 
Workbench with all ch.zhaw. plugins included.

# Building
## Linux
Install Qt5 (5.0 - 5.4, 5.5 not yet supported) with your package manager. Download source, configure with CMake and `make`. 
## OSX
Install Qt4 (Qt5 not yet supported) either via installer at https://www.qt.io/download/ or with brew. Download source, configure with CMake and `make`.
## Windows
You need a very specific setup of tools to make this work on Windows... Follow the instructions here http://www.mitk.org/wiki/Developer_Tutorial_(Microsoft_Windows)
- MSVC 2012v4 or MSVC 2013 (e.g. Visual Studio 2013 Community Edition). ** MSVC 2015 IS NOT SUPPORTED**
- Qt 5.0 - 5.4 ** WITH OpenGL ENABLED **. When using the Qt binary installer, pick the `msvc2013 64-bit OpenGL` build. Every other build will not work!!!
- CMake 3.2
- Save the source and the build in a ** very short ** top-level directory (about 10 characters for the total path).
- Once you configured the project with CMake and generate a visual studio solution, build the `ALL_BUILD` target.

# Issue with QT5 / QT4
There's an ongoing issue with Qt versions in the current release. MITK decided to make Qt5 the default version on all 
platforms even tho the QT5 build on OSX does not work properly (reported issue). Hopefully this will get fixed with the
next release. For now, don' try to make a OSX build with Qt5 - you may get it to compile, but you will have some runtime
issue with window resizing and colors.