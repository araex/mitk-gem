#include <mitkLogMacros.h>

#include "Runner.h"
#include "GridComparator.h"

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

using namespace Testing;

void Runner::runUnitTests() {
    Catch::Session().run();
}

void Runner::compareGrids(mitk::UnstructuredGrid::Pointer _mitkUgrid0, mitk::UnstructuredGrid::Pointer _mitkUGrid1) {
    MITK_INFO("TESTING") << "comparing unstructured grids...";

    auto ugrid0 = _mitkUgrid0->GetVtkUnstructuredGrid();
    auto ugrid1 = _mitkUGrid1->GetVtkUnstructuredGrid();

    std::stringstream outputStream;
    GridComparator comparator(ugrid0, ugrid1, &outputStream);
    MITK_INFO("ch.zhaw.materialmapping") << outputStream.str();
}