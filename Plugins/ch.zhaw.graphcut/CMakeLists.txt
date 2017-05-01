INCLUDE (CheckIncludeFiles)

project(ch_zhaw_graphcut)

mitk_create_plugin(
  EXPORT_DIRECTIVE GRAPHCUT_EXPORT
  EXPORTED_INCLUDE_SUFFIXES src
  MODULE_DEPENDS MitkQtWidgetsExt
)

# check for gridCut library
set(GridCutDir ${CMAKE_CURRENT_SOURCE_DIR}/src/internal/lib/GraphCut3D/lib/gridcut)
if(EXISTS "${GridCutDir}/include/GridCut/GridGraph_3D_6C_MT.h")
    message("Found GridCut library")
    set(GRIDCUT_LIBRARY_AVAILABLE 1)
    include_directories(src/internal/lib/GraphCut3D/lib/gridcut/include)
else()
    message("GridCut library not found. In order to use GridCut, download a copy at http://www.gridcut.com and extract the content to: " ${GridCutDir})
endif()
CONFIGURE_FILE(${GridCutDir}/config.h.in ${GridCutDir}/config.h)