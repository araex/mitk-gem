![MITK-GEM logo](https://github.com/araex/mitk-gem/blob/master/mitk_gem.png)

MITK - Generate Models (MITK-GEM) is a free open-source software pipeline to generate finite element models from image data. 
It is built on top of the [Medical Imaging Interaction Toolkit (MITK)](http://mitk.org/wiki/MITK) Workbench.

# Features
- Interactive image segmentation with 3D graph cuts
- Surface mesh generation
- Tetrahedral mesh generation
- Material mapping

For more information, check our [website](http://araex.github.io/mitk-gem-site/) and our [simtk project](https://simtk.org/home/mitk-gem).

# Download
You can find binaries of the MITK-GEM application on our [downloads](https://simtk.org/project/xml/downloads.xml?group_id=1063) page.

If you'd like to build MITK-GEM from source, follow the instructions below.

# Dependencies
- CMake >=3.2. cmake-gui is recommended.
- QT 5.5. ** QT 5.6 is not supported! **

# Supported compilers
The versions of supported compilers are:
- MSVC 2012 Update 4 ** MSVC 2015 is not supported! **
- GNU 4.7.3
- Clang 3.4
- AppleClang 5.0
Newer versions might work, but have not been tested.

# Prerequisites
## Ubuntu (>= 14.04 LTS)
- Install dependencies: `sudo apt-get install git build-essential cmake-gui libxt-dev libtiff4-dev libwrap0-dev libgmp3-dev libcgal-dev mesa-common-dev freeglut3-dev`
- [Qt 5.5](https://download.qt.io/new_archive/qt/5.5/)

### Ubuntu 12.04 LTS
Ubuntu 12.04 bundles cmake version 2.8.7, but you will need 3.4 in order to build MITK-GEM. A manual update is required.
- Install [CMake 3.2 or later](http://www.cmake.org/download/)
- Install dependencies: `sudo apt-get install git build-essential libxt-dev libtiff4-dev libwrap0-dev libgmp3-dev libcgal-dev mesa-common-dev freeglut3-dev`
- [Qt 5.5](https://download.qt.io/new_archive/qt/5.5/)

## Mac OSX
- Apple Xcode https://developer.apple.com/xcode/ with activated command line tools. To do so, open Xcode, go to Preferences -> Downloads -> Components -> Command Line Tools.
- Most recent version of the Xcode command line tools `xcode-select --install`.
- [Qt 5.5](https://download.qt.io/new_archive/qt/5.5/)
- [CMake 3.2 or later](http://www.cmake.org/download/)
- [GMP](https://gmplib.org/#DOWNLOAD). We recommend using [homebrew](http://brew.sh/) `brew install gmp`

## Windows
- MSVC 2012v4 or MSVC 2013 (e.g. Visual Studio 2013 Community Edition). ** MSVC 2015 IS NOT SUPPORTED**
- [QT 5.5 msvc2013](https://download.qt.io/new_archive/qt/5.5/) ** WITH OpenGL ENABLED **. When using the Qt binary installer, pick the `msvc2013 64-bit OpenGL` build.
- [CMake 3.2 or later](http://www.cmake.org/download/)
- [GMP](https://gmplib.org/#DOWNLOAD). We recommend getting a prebuilt version using the [CGAL installer](https://github.com/CGAL/cgal/releases/download/releases%2FCGAL-4.9/CGAL-4.9-Setup.exe)

# Build instructions
## Linux & OSX
1. Download the source code `git clone https://github.com/araex/mitk-gem src`
2. Create a new directory for the build `mkdir build`
3. Change to the build directory and run CMake `cd build; cmake ../src`
4. Start the build `make -j 8`. This will take a long time (up to an hour) and requires internet access.
5. Once compilation is finished, you can find the executable `MITK-GEM` in the directory `build/MITK-GEM-build/bin/`

## Windows
Follow the instructions here http://www.mitk.org/wiki/Developer_Tutorial_(Microsoft_Windows)".
Remember to save the source and the build in a ** very short ** top-level directory.

## Optional features
### Using GridCut
[GridCut](http://www.gridcut.com/) is an alternative min-cut / max-flow solver to use instead of [maxflow](https://pub.ist.ac.at/~vnk/software.html) in the GraphCut3D plugin. We've seen significant performance increase  in both speed and memory efficiency. For licensing reasons we cannot redistribute GridCut (neither in the source nor binary release), so you have to download and install it manually:
1. Go to the [GridCut website](http://www.gridcut.com/) and download the source code.
2. Copy the contents of the .zip archive to mitk-gem source code directory `Plugins/ch.zhaw.graphcut/src/internal/lib/GraphCut3D/lib/gridcut`
3. Build `make -j 8`

# FAQ
For questions regarding the usage of MITK-GEM, refer to our [application FAQ](http://araex.github.io/mitk-gem-site/#faq).
## The compile process has stopped at 'Updating MITK'
This step pulls the most recent version of MITK from our MITK [fork](https://github.com/araex/mitk). 
Depending on the server and your internet connection, this might take some time. 
If you're using a proxy, please confirm that git is [configured](http://stackoverflow.com/questions/783811/getting-git-to-work-with-a-proxy-server) correctly.

## CMake Error: Found unsuitable Qt version "" from NOTFOUND
You need to install Nokias QT library. See [http://qt-project.org/downloads](http://qt-project.org/downloads) for instructions.

# License
MITK-GEM is released under the GPLv3 license. However, the plugins are individually licensed and may be used under these conditions separately:
- Material Mapping: BSD 3-Clause License.
- Surface Mesher: BSD 3-Clause License
- Resample Plugin: BSD 3-Clause License
- Padding Plugin: BSD 3-Clause License
- GraphCut3D: GPLv3 because of [maxflow](https://pub.ist.ac.at/~vnk/software.html).
- Volume Mesher: GPLv3 because of [tetgen](http://wias-berlin.de/software/tetgen/).

