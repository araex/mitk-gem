#pragma once

#include <mitkImage.h>
#include <mitkUnstructuredGridToUnstructuredGridFilter.h>

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkImageStencil.h>

#include "BoneDensityFunctor.h"
#include "PowerLawFunctor.h"

class MaterialMappingFilter : public mitk::UnstructuredGridToUnstructuredGridFilter {
public:
    mitkClassMacro(MaterialMappingFilter, UnstructuredGridToUnstructuredGridFilter)
    itkFactorylessNewMacro(Self)
    itkCloneMacro(Self)

    void SetIntensityImage(mitk::Image::Pointer _p){
        m_IntensityImage = _p;
    }

    void SetDensityFunctor(BoneDensityFunctor &&_f){
        m_BoneDensityFunctor = _f;
    }

    void SetPowerLawFunctor(PowerLawFunctor &&_f){
        m_PowerLawFunctor = _f;
    }

    virtual void GenerateData() override;

protected:
    using VtkImage = vtkSmartPointer<vtkImageData>;
    using VtkStencil = vtkSmartPointer<vtkImageStencil>;
    using VtkUGrid = vtkSmartPointer<vtkUnstructuredGridBase>;
    using VtkDoubleArray = vtkSmartPointer<vtkDoubleArray>;

    MaterialMappingFilter();
    virtual ~MaterialMappingFilter(){};

    VtkUGrid extractSurface(const VtkUGrid);
    VtkImage extractVOI(const VtkImage, const VtkUGrid);
    VtkImage createStencil(const VtkUGrid, const VtkImage); // convert surface to inverted binary mask (=> 0 inside, 1 outside)
    VtkImage createPeeledMask(const VtkImage _img, const VtkImage _mask);
    void inplaceExtendImage(VtkImage _img, VtkImage _mask, bool _maxVal); // weighted average in neighborhood, performed in place
    VtkDoubleArray evaluateFunctorsForNodes(const VtkUGrid, const VtkImage, std::string _name, double _minElem); // "interpolateToNodes". evaluates both functors for each vertex of the mesh
    VtkDoubleArray nodesToElements(const VtkUGrid, VtkDoubleArray _nodeData, std::string _name);

    mitk::Image::Pointer m_IntensityImage;
    BoneDensityFunctor m_BoneDensityFunctor;
    PowerLawFunctor m_PowerLawFunctor;
};