#pragma once

#include <mitkUnstructuredGrid.h>

namespace Testing {
    class Runner {
    public:
        void compareGrids(mitk::UnstructuredGrid::Pointer, mitk::UnstructuredGrid::Pointer);
    };
}