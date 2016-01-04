#pragma once
#include <sstream>

#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

class GridComparator {
public:
    using UgridPtr = vtkSmartPointer<vtkUnstructuredGrid>;
    GridComparator() = delete;
    GridComparator(UgridPtr, UgridPtr, std::stringstream*);

private:
    bool compareCellStructure();
    void compareCellData();
    void comparePointData();

    void compareArrayStructure(vtkDataArray *, vtkDataArray *, std::string _logPrefix);
    void compareArrayData(vtkDataArray *, vtkDataArray *, std::string _logPrefix);

    void printBuf(double *, int);
    UgridPtr u0, u1;
    std::stringstream *out;
};