#pragma once

#include <vtkSmartPointer.h>

class vtkPolyData;
class vtkUnstructuredGrid;

namespace gem
{
    /*!
     * Interface for algorithms to calculate a volume mesh given a surface using vtk data types.
     * @author Thomas Fitze
     */
    class IMesher
    {
    public:
        //! Sets the input surface to be meshed
        void SetInput(vtkSmartPointer<vtkPolyData> spSurface) {m_spSurface = spSurface;};

        //! Sets the output mesh
        void SetOutput(vtkSmartPointer<vtkUnstructuredGrid> spUGrid) {m_spUGrid = spUGrid;};

        //! Returns the output mesh
        vtkSmartPointer<vtkUnstructuredGrid> GetOutput(void) const { return m_spUGrid; };

        /*!
         * Computes the volume mesh
         *
         * @precondition   Input surface is set
         */
        void Compute();

    protected:
        //! Mesher specific compute implementation
        virtual void compute() = 0;

        vtkSmartPointer<vtkPolyData>            m_spSurface;    //!< Input Surface
        vtkSmartPointer<vtkUnstructuredGrid>    m_spUGrid;      //!< Output
    };
}
