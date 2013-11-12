ImageGraphCut3DSegmentation
===========================

An extensions of ITKs (D Doria) 2D Graph cut segmentation to 3D grayscale images


Getting the code
----------------


Overview
--------
This software allows the user to perform a foreground/background segmentation of an image.
This implementation is based on "Graph Cuts and Efficient N-D Image Segmentation" by Yuri Boykov (IJCV 2006).

License
--------
GPLv3 (See LICENSE.txt). This is required because of the use of Kolmogorov's code.

Build notes
------------------
This code depends on c++0x/11 additions to the c++ language. For Linux, this means it must be built with the flag
gnu++0x (or gnu++11 for gcc >= 4.7).

$ mkdir build
$ cd build
$ ccmake ../ -DCMAKE_CXX_FLAGS=-std=gnu++11 -DCMAKE_C_COMPILER=/usr/bin/gcc-4.7 -DCMAKE_CXX_COMPILER=/usr/bin/g++-4.7
  'c'
  set Release
  set /home/visualcomputing/code/InsightToolkit-4.4.2/build
  'c'
  Set build tests to ON
  Set build examples to ON
  'c'
  'g'
$ make


Dependencies
------------
- ITK >= 4

Example
-------
$ cd data
$ ../build/Examples/ImageGraphCut3DSegmentationExample testCube10x10x10.mhd testForegroundMask.mhd testBackgroundMask.mhd testSegmentedCube.mhd -1

$../build/Examples/ImageGraphCut3DSegmentationExample testCube10x10x10Noisy_0p01.mhd testForegroundMask.mhd testBackgroundMask.mhd testSegmentedNoisyCube.mhd -1

