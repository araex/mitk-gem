# Description 
Workbench with all ch.zhaw. plugins included.

# Issue with QT5 / QT4
There's an ongoing issue with QT versions in the current release. MITK decided to make QT5 the default version on all 
platforms even tho the QT5 build on OSX does not work properly (reported issue). Hopefully this will get fixed with the
next release. For now, don' try to make a OSX build with QT5 - you may get it to compile, but you will have some runtime
issue with window resizing and colors.