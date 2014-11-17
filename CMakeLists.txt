cmake_minimum_required(VERSION 2.8)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

project(ImageGraphCut3DSegmentation)

# ITK
FIND_PACKAGE(ITK REQUIRED ITKCommon ITKIOImageBase ITKIOPNG ITKIOMeta
                          ITKImageIntensity ITKImageFeature
                          ITKMathematicalMorphology
                          ITKBinaryMathematicalMorphology ITKDistanceMap
                          ITKTestKernel)
INCLUDE( ${USE_ITK_FILE} )

if( "${ITK_VERSION_MAJOR}" LESS 4 )
  MESSAGE(FATAL_ERROR "You must build ImageGraphCutSegmentation with ITK >= 4.0!")
endif( "${ITK_VERSION_MAJOR}" LESS 4 )

# Set include directory
set(ImageGraphCut3DSegmentation_include_dirs ${ImageGraphCut3DSegmentation_include_dirs} ${CMAKE_CURRENT_SOURCE_DIR})

# Build the graph cut library
add_subdirectory(lib/kolmogorov-3.03)
set(ImageGraphCut3DSegmentation_libraries ${ImageGraphCut3DSegmentation_libraries} KolmogorovMaxFlow)

# Give the compiler all of the required include directories
include_directories(${ImageGraphCut3DSegmentation_include_dirs})

# Options
option(ImageGraphCut3DSegmentation_BuildExamples "Build ImageGraphCut3DSegmentation examples?" ON)
if(ImageGraphCut3DSegmentation_BuildExamples)
  add_subdirectory(Examples)
endif()

option(ImageGraphCut3DSegmentation_BuildTests "Build ImageGraphCut3DSegmentation tests?" OFF)
if(ImageGraphCut3DSegmentation_BuildTests)
  # gtest
  add_subdirectory(lib/gtest-1.7.0)
  enable_testing()
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
  include_directories(src)

  # experimental max flow libraries
  set(Boost_USE_STATIC_LIBS OFF)
  set(Boost_USE_MULTITHREADED ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
  find_package(Boost 1.55.0 COMPONENTS graph)

  if(Boost_FOUND)
      include_directories(${Boost_INCLUDE_DIRS})
  endif()

  # build the tests
  subdirs(test)
endif()

