#pragma once
#include <sstream>

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <cmath>

class GridComparator {
public:
    using UgridPtr = vtkSmartPointer<vtkUnstructuredGrid>;
    GridComparator() = delete;
    GridComparator(UgridPtr, UgridPtr, std::stringstream*);

private:
    void compareArrayStructure(vtkDataArray *, vtkDataArray *, std::string _logPrefix);
    void compareArrayData(vtkDataArray *, vtkDataArray *, std::string _logPrefix);

    void printBuf(double *, int);
    UgridPtr u0, u1;
    std::stringstream *out;

    bool almostEqual(double a, double b) {
        return std::abs(a - b) < 0.0001;
    }
};