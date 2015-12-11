ImageGraphCut3DSegmentation
===========================
An extensions of ITKs (D Doria) 2D Graph cut segmentation to 3D grayscale images

Overview
--------
This software allows the user to perform a foreground/background segmentation of a 3-dimensional grayscale image. This 
implementation is based on "Graph Cuts and Efficient N-D Image Segmentation" by Yuri Boykov (IJCV 2006) and David Dorias
"ImageGraphCutSegmentation" 2-dimensional implementation of the same paper.

Getting the code
----------------
This repository uses git submodules to manage its dependencies. Because of this, after cloning the repository, you will
need to run `git submodule update --init --recursive` to pull all submodules. Updating to a new version requires a
`git submodule update --recursive`.


Build notes
------------------
```
$ mkdir build
$ cd build
$ ccmake ../ 
  'c'
  set Release
  set /home/visualcomputing/code/InsightToolkit-4.4.2/build
  'c'
  Set build tests to OFF
  Set build examples to ON
  'c'
  'g'
$ make
```

Dependencies
------------
- ITK >= 4

If tests are enabled: 
- Boost >=1.55

Example
-------
```
$ cd data/femur

$ ../../build/Examples/ImageGraphCut3DSegmentationExample input.mhd foreground.mhd background.mhd result.mhd 50 0 1

```

License
--------
GPLv3 (See LICENSE.txt). This is required because of the use of Kolmogorovs code.