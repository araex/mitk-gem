Interactive GraphCut 3D Segmentation Workbench
==============================================
This software allows the user to perform an interactive foreground/background segmentation of a 3-dimensional grayscale image. It is implemented as plugin for the MITK-Workbench.

# Introduction
Please read through the following instructions carefully! Not following the instructions or skipping steps will result in build errors or missing features. If you have any issues, please consult the FAQ at the end of this document.

# Dependencies
- CMake >=3.2 cmake-gui is recommended.
- QT 4.8 (OSX), QT 5 (Windows & Linux)

# Prerequisites
## on Ubuntu
If you use the latest Ubuntu release (>= 14.04 LTS), you can install all the dependencies with the following command:
`sudo apt-get install git build-essential cmake-gui qt-sdk libxt-dev libtiff4-dev libwrap0-dev`
### on Ubuntu 12.04 LTS
Ubuntu 12.04 bundles cmake version 2.8.7, but you will need 3.2 in order to build MITK. A manual update is required.

1. Install dependencies with the following command: `sudo apt-get install git build-essential qt-sdk libxt-dev libtiff4-dev libwrap0-dev`
2. Go to http://www.cmake.org/download/
3. Download and install a more recent (>= 3.2) version of cmake.

## on Mac OSX
- Install Apple Xcode https://developer.apple.com/xcode/ and activate the command line tools. To do so, open Xcode, go to Preferences -> Downloads -> Components -> Command Line Tools.
- Install QT 4.8 http://download.qt-project.org/archive/qt/
- Install CMake http://www.cmake.org/download/

# Building the workbench
Once you've downloaded the source code and updated the submodules, you can start configuring the build with CMake. We are using the MITK Superbuild to automate as much as possible. However, there are a couple of steps you have to take manually - not doing so will result in missing functionality or build errors! Please follow both instructions, Superbuild and Configuring MITK-build, carefully:

## Superbuild
1. Create a new directory `mkdir MITK-GEM-build`.
2. Open CMake.
3. Set the CMake build directory to the one you created in step 1 `MITK-GEM-build`.
4. Set the CMake source directory to the repository you downloaded `MITK-GEM-source`.
5. Click `configure`. CMake will ask you about your generator, for this instruction we will use `Unix Makefile`.
6. Click `configure` repeatedly until CMake shows no more new values (red lines).
7. Click `generate`. This will create a Makefile in your build directory.
8. Change to your build directory `cd GraphCut3D-build`.
9. Start the build `make -j 8`. This can take a long time (more than 1 hour) and requires internet access.
10. Everything is set up and your workbench should be ready to use. You can find the executable `MITK-GEM.app` in the directory `MITK-GEM-build/MITK-GEM-build/bin/`.

# FAQ
### What is MITK?
The framework that runs our plugins. See http://www.mitk.org/
### The compile process has stopped at 'Updating MITK'
This step pulls the most recent version of MITK from the official git repository. Depending on the server and your internet connection, this might take some time. Please ensure your internet connection is working and try again later.
### CMake Error: Found unsuitable Qt version "" from NOTFOUND
You need to install Nokias QT library. See http://qt-project.org/downloads for instructions.

# License
This is a redistribution of the MITK-Workbench that includes the GraphCut3D Segmentation plugin. The MITK-Workbench has not been modified and is distributed as-is. License information about MITK can be found in `LICENSE`.

The included plugin ch.zhaw.graphcut is release under GPLv3 (see `Plugins/ch.zhaw.graphcut/LICENSE`) because of the use of Kolmogorovs MAXFLOW library.
