#-----------------------------------------------------------------------------
# MITK
#-----------------------------------------------------------------------------

set(MITK_DEPENDS)
set(proj_DEPENDENCIES)
set(proj MITK)

if(NOT MITK_DIR)

  #-----------------------------------------------------------------------------
  # Create CMake options to customize the MITK build
  #-----------------------------------------------------------------------------

  option(MITK_USE_SUPERBUILD "Use superbuild for MITK" ON)
  option(MITK_USE_BLUEBERRY "Build the BlueBerry platform in MITK" ON)
  option(MITK_BUILD_TUTORIAL "Build the MITK tutorial" OFF)
  option(MITK_BUILD_ALL_PLUGINS "Build all MITK plugins" ON)
  option(MITK_BUILD_TESTING "Build the MITK unit tests" OFF)
  option(MITK_USE_CTK "Use CTK in MITK" ${MITK_USE_BLUEBERRY})
  option(MITK_USE_DCMTK "Use DCMTK in MITK" ON)
  option(MITK_USE_QT "Use Nokia's Qt library in MITK" ON)
  option(MITK_USE_Boost "Use the Boost library in MITK" OFF)
  option(MITK_USE_OpenCV "Use Intel's OpenCV library" OFF)
  option(MITK_USE_Python "Enable Python wrapping in MITK" OFF)

  if(MITK_USE_BLUEBERRY AND NOT MITK_USE_CTK)
    message("Forcing MITK_USE_CTK to ON because of MITK_USE_BLUEBERRY")
    set(MITK_USE_CTK ON CACHE BOOL "Use CTK in MITK" FORCE)
  endif()

  if(MITK_USE_CTK AND NOT MITK_USE_QT)
    message("Forcing MITK_USE_QT to ON because of MITK_USE_CTK")
    set(MITK_USE_QT ON CACHE BOOL "Use Nokia's Qt library in MITK" FORCE)
  endif()

  set(MITK_USE_CableSwig ${MITK_USE_Python})
  set(MITK_USE_GDCM 1)
  set(MITK_USE_ITK 1)
  set(MITK_USE_VTK 1)

  mark_as_advanced(MITK_USE_SUPERBUILD
                   MITK_BUILD_ALL_PLUGINS
                   MITK_BUILD_TESTING
                   )

  set(mitk_cmake_boolean_args
    MITK_USE_SUPERBUILD
    MITK_USE_BLUEBERRY
    MITK_BUILD_TUTORIAL
    MITK_BUILD_ALL_PLUGINS
    MITK_USE_CTK
    MITK_USE_DCMTK
    MITK_USE_QT
    MITK_USE_Boost
    MITK_USE_OpenCV
    MITK_USE_Python
   )

  set(DESIRED_QT_VERSION 5)

  if(MITK_USE_QT)
    # Look for Qt at the superbuild level, to catch missing Qt libs early
    if(APPLE)
	  set(DESIRED_QT_VERSION 4)
      find_package(Qt4 4.7 REQUIRED)
	else()
	  find_package(Qt5
	    5.0.0
	    COMPONENTS
		  Concurrent
		  OpenGL
		  PrintSupport
		  Script
		  Sql
		  Svg
		  WebKitWidgets
		  Xml
		  XmlPatterns
		  UiTools
		REQUIRED)
	endif()
  endif()

  set(additional_mitk_cmakevars
    -DDESIRED_QT_VERSION:STRING=${DESIRED_QT_VERSION}
  )

  # Configure the set of default pixel types
  set(MITK_ACCESSBYITK_INTEGRAL_PIXEL_TYPES
      "int, unsigned int, short, unsigned short, char, unsigned char"
      CACHE STRING "List of integral pixel types used in AccessByItk and InstantiateAccessFunction macros")

  set(MITK_ACCESSBYITK_FLOATING_PIXEL_TYPES
      "double, float"
      CACHE STRING "List of floating pixel types used in AccessByItk and InstantiateAccessFunction macros")

  set(MITK_ACCESSBYITK_COMPOSITE_PIXEL_TYPES
      ""
      CACHE STRING "List of composite pixel types used in AccessByItk and InstantiateAccessFunction macros")

  set(MITK_ACCESSBYITK_DIMENSIONS
      "2,3"
      CACHE STRING "List of dimensions used in AccessByItk and InstantiateAccessFunction macros")

  foreach(_arg MITK_ACCESSBYITK_INTEGRAL_PIXEL_TYPES MITK_ACCESSBYITK_FLOATING_PIXEL_TYPES
               MITK_ACCESSBYITK_COMPOSITE_PIXEL_TYPES MITK_ACCESSBYITK_DIMENSIONS)
    mark_as_advanced(${_arg})
    list(APPEND additional_mitk_cmakevars "-D${_arg}:STRING=${${_arg}}")
  endforeach()

  #-----------------------------------------------------------------------------
  # Create options to inject pre-build dependencies
  #-----------------------------------------------------------------------------

  foreach(proj CTK DCMTK GDCM VTK ITK OpenCV CableSwig)
    if(MITK_USE_${proj})
      set(MITK_${proj}_DIR "${${proj}_DIR}" CACHE PATH "Path to ${proj} build directory")
      mark_as_advanced(MITK_${proj}_DIR)
      if(MITK_${proj}_DIR)
        list(APPEND additional_mitk_cmakevars "-D${proj}_DIR:PATH=${MITK_${proj}_DIR}")
      endif()
    endif()
  endforeach()

  if(MITK_USE_Boost)
    set(MITK_BOOST_ROOT "${BOOST_ROOT}" CACHE PATH "Path to Boost directory")
    mark_as_advanced(MITK_BOOST_ROOT)
    if(MITK_BOOST_ROOT)
      list(APPEND additional_mitk_cmakevars "-DBOOST_ROOT:PATH=${MITK_BOOST_ROOT}")
    endif()
  endif()

  set(MITK_SOURCE_DIR "" CACHE PATH "MITK source code location. If empty, MITK will be cloned from MITK_GIT_REPOSITORY")
  set(MITK_GIT_REPOSITORY "http://git.mitk.org/MITK.git" CACHE STRING "The git repository for cloning MITK")
  set(MITK_GIT_TAG "releases/2015-05-02" CACHE STRING "The git tag/hash to be used when cloning from MITK_GIT_REPOSITORY")
  set(MITK_WHITELIST "VCLab")
  mark_as_advanced(MITK_SOURCE_DIR MITK_GIT_REPOSITORY MITK_GIT_TAG)

  #-----------------------------------------------------------------------------
  # Create the final variable containing superbuild boolean args
  #-----------------------------------------------------------------------------

  set(mitk_boolean_args)
  foreach(mitk_cmake_arg ${mitk_cmake_boolean_args})
    list(APPEND mitk_boolean_args -D${mitk_cmake_arg}:BOOL=${${mitk_cmake_arg}})
  endforeach()

  #-----------------------------------------------------------------------------
  # pass the whitelist to MITK build
  #-----------------------------------------------------------------------------
  MESSAGE( STATUS "WHITELIST VARIABLES ON EXTERNAL MITK LEVEL" )
  MESSAGE( STATUS "WHITELIST:                          " ${MITK_WHITELIST} )
  MESSAGE( STATUS "MITK_WHITELISTS_EXTERNAL_PATH:      " ${MITK_WHITELISTS_EXTERNAL_PATH} )
  MESSAGE( STATUS "MITK_WHITELISTS_INTERNAL_PATH:      " ${MITK_WHITELISTS_INTERNAL_PATH} )

  get_filename_component(MITK_WHITELISTS_INTERNAL_PATH ${MITK_WHITELISTS_INTERNAL_PATH} ABSOLUTE)
  list(APPEND additional_mitk_cmakevars "-DMITK_WHITELIST:STRING=${MITK_WHITELIST}")
  list(APPEND additional_mitk_cmakevars "-DMITK_WHITELISTS_EXTERNAL_PATH:FILEPATH=${MITK_WHITELISTS_EXTERNAL_PATH}")
  list(APPEND additional_mitk_cmakevars "-DMITK_WHITELISTS_INTERNAL_PATH:FILEPATH=${MITK_WHITELISTS_INTERNAL_PATH}")
  list(APPEND additional_mitk_cmakevars "-DMITK_BUILD_ALL_PLUGINS:BOOL=${MITK_BUILD_ALL_PLUGINS}")

  #-----------------------------------------------------------------------------
  # Additional MITK CMake variables
  #-----------------------------------------------------------------------------

  if(MITK_USE_QT AND QT_QMAKE_EXECUTABLE)
    list(APPEND additional_mitk_cmakevars "-DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}")
  endif()

  if(MITK_USE_CTK)
    list(APPEND additional_mitk_cmakevars "-DGIT_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE}")
  endif()

  if(MITK_INITIAL_CACHE_FILE)
    list(APPEND additional_mitk_cmakevars "-DMITK_INITIAL_CACHE_FILE:INTERNAL=${MITK_INITIAL_CACHE_FILE}")
  endif()

  if(MITK_USE_SUPERBUILD)
    set(MITK_BINARY_DIR ${proj}-superbuild)
  else()
    set(MITK_BINARY_DIR ${proj}-build)
  endif()

  set(proj_DEPENDENCIES)
  set(MITK_DEPENDS ${proj})

  # Configure the MITK souce code location

  if(NOT MITK_SOURCE_DIR)
    set(mitk_source_location
        SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
        GIT_REPOSITORY ${MITK_GIT_REPOSITORY}
        GIT_TAG ${MITK_GIT_TAG}
        )
  else()
    set(mitk_source_location
        SOURCE_DIR ${MITK_SOURCE_DIR}
       )
  endif()

  ExternalProject_Add(${proj}
    ${mitk_source_location}
    BINARY_DIR ${MITK_BINARY_DIR}
    PREFIX ${proj}${ep_suffix}
    INSTALL_COMMAND ""
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${ep_common_args}
      ${mitk_boolean_args}
      ${additional_mitk_cmakevars}
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DBUILD_TESTING:BOOL=${MITK_BUILD_TESTING}
    CMAKE_CACHE_ARGS
      ${ep_common_cache_args}
    CMAKE_CACHE_DEFAULT_ARGS
      ${ep_common_cache_default_args}
    DEPENDS
      ${proj_DEPENDENCIES}
    )

  if(MITK_USE_SUPERBUILD)
    set(MITK_DIR "${CMAKE_CURRENT_BINARY_DIR}/${MITK_BINARY_DIR}/MITK-build")
  else()
    set(MITK_DIR "${CMAKE_CURRENT_BINARY_DIR}/${MITK_BINARY_DIR}")
  endif()

else()

  # The project is provided using MITK_DIR, nevertheless since other
  # projects may depend on MITK, let's add an 'empty' one
  MacroEmptyExternalProject(${proj} "${proj_DEPENDENCIES}")

  # Further, do some sanity checks in the case of a pre-built MITK
  set(my_itk_dir ${ITK_DIR})
  set(my_vtk_dir ${VTK_DIR})
  set(my_qmake_executable ${QT_QMAKE_EXECUTABLE})

  find_package(MITK REQUIRED)

  if(my_itk_dir AND ITK_DIR)
    if(NOT my_itk_dir STREQUAL ${ITK_DIR})
      message(FATAL_ERROR "ITK packages do not match:\n   ${MY_PROJECT_NAME}: ${my_itk_dir}\n  MITK: ${ITK_DIR}")
    endif()
  endif()

  if(my_vtk_dir AND VTK_DIR)
    if(NOT my_vtk_dir STREQUAL ${VTK_DIR})
      message(FATAL_ERROR "VTK packages do not match:\n   ${MY_PROJECT_NAME}: ${my_vtk_dir}\n  MITK: ${VTK_DIR}")
    endif()
  endif()

  if(my_qmake_executable AND MITK_QMAKE_EXECUTABLE)
    if(NOT my_qmake_executable STREQUAL ${MITK_QMAKE_EXECUTABLE})
      message(FATAL_ERROR "Qt qmake does not match:\n   ${MY_PROJECT_NAME}: ${my_qmake_executable}\n  MITK: ${MITK_QMAKE_EXECUTABLE}")
    endif()
  endif()

endif()
