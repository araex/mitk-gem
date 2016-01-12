![MITK-GEM logo](https://github.com/araex/mitk-gem/blob/material-mapping/mitk_gem.png)
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
- CMake >=3.2 cmake-gui is recommended.
- QT 4.8 (OSX), QT 5 (Windows & Linux)

# Prerequisites
## on Ubuntu
If you use the latest Ubuntu release (>= 14.04 LTS), you can install all the dependencies with the following command:
`sudo apt-get install git build-essential cmake-gui qt-sdk libxt-dev libtiff4-dev libwrap0-dev`

### on Ubuntu 12.04 LTS
Ubuntu 12.04 bundles cmake version 2.8.7, but you will need 3.2 in order to build MITK. A manual update is required.

1. Install dependencies: `sudo apt-get install git build-essential qt-sdk libxt-dev libtiff4-dev libwrap0-dev`
2. Go to http://www.cmake.org/download/
3. Download and install a more recent (>= 3.2) version of cmake.

## on Mac OSX
- Install Apple Xcode https://developer.apple.com/xcode/ and activate the command line tools. To do so, open Xcode, go to Preferences -> Downloads -> Components -> Command Line Tools.
- Install QT 4.8 http://download.qt-project.org/archive/qt/
- Install CMake http://www.cmake.org/download/

# Build instructions Linux & OSX
1. Download the source code `git clone https://github.com/araex/mitk-gem src`.
2. Create a new directory for the build `mkdir build`.
3. Change to the build directory and run CMake `cd build; cmake ../src`
4. Start the build `make -j 8`. This will take a long time (up to an hour) and requires internet access.
5. Once compilation is finished, you can find the executable `MITK-GEM` in the directory `build/MITK-GEM-build/bin/`.

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
- GraphCut3D: GPLv3 because of the used library [maxflow](https://pub.ist.ac.at/~vnk/software.html).
- Volume Mesher: GPLv3 beaucse of the used library [tetgen](http://wias-berlin.de/software/tetgen/).

