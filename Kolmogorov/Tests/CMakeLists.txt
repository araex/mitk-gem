# Allow headers in tests to be included like
# #include "graph.h" rather than needing
# #include "../graph.h"
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

ADD_EXECUTABLE(MaxflowTest MaxflowTest.cpp)
TARGET_LINK_LIBRARIES(MaxflowTest KolmogorovMaxFlow)
