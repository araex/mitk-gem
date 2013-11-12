ImageGraphCut3DSegmentation
===========================

An extensions of ITKs (D Doria) 2D Graph cut segmentation to 3D grayscale images

From the original Readme.txt

Getting the code
----------------
After you have cloned this repository, you will need to initialize the submodules:
git submodule update --init --recursive

If you ever update with 'git pull origin master', you must then do 'git submodule update --recursive'.

This pulls in all of the dependencies including Mask (which includes ITKHelpers and then Helpers).

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

Dependencies
------------
- ITK >= 4
