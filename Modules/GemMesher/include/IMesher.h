#pragma once

#include <vtkSmartPointer.h>

class vtkPolyData;
class vtkUnstructuredGrid;

class IMesher
{
public:
    void SetInput(vtkSmartPointer<vtkPolyData> spSurface) {m_spSurface = spSurface;};
    void SetOutput(vtkSmartPointer<vtkUnstructuredGrid> spUGrid) {m_spUGrid = spUGrid;};
    vtkSmartPointer<vtkUnstructuredGrid> GetOutput(void) const { return m_spUGrid; };

    void Compute();

protected:
    virtual void compute() = 0;
    vtkSmartPointer<vtkPolyData>            m_spSurface;    //!< Input Surface
    vtkSmartPointer<vtkUnstructuredGrid>    m_spUGrid;      //!< Output
};

template<class Mesher>
void CreateVolumeMeshFromSurface(vtkSmartPointer<vtkPolyData> spSurface, vtkSmartPointer<vtkUnstructuredGrid> spUGrid)
{
    static_assert(std::is_base_of<IMesher, Mesher>::value, "Mesher needs to implement interface IMesher");
    Mesher mesher;
    mesher.SetInput(spSurface);
    mesher.SetOutput(spUGrid);
    mesher.Compute();
}