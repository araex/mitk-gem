#pragma once

#include <mitkImage.h>
#include <mitkUnstructuredGridToUnstructuredGridFilter.h>

class MaterialMappingFilter : public mitk::UnstructuredGridToUnstructuredGridFilter {
public:
    using TFunctor = std::function<double(double)>; // TODO: might want to make this a template parameter of the filter?
    mitkClassMacro(MaterialMappingFilter, UnstructuredGridToUnstructuredGridFilter)
    itkFactorylessNewMacro(Self)
    itkCloneMacro(Self)

    void SetIntensityImage(mitk::Image::Pointer _p){
        m_IntensityImage = _p;
    }

    void SetLinearFunctor(TFunctor &&_f){
        m_LinearFunctor = _f;
    }

    void SetPowerLawFunctor(TFunctor &&_f){
        m_PowerLawFunctor = _f;
    }

    virtual void GenerateOutputInformation() override;
    virtual void GenerateData() override;
protected:
    MaterialMappingFilter();
    virtual ~MaterialMappingFilter(){};

    vtkSmartPointer<vtkDoubleArray> createDataArray(std::string);
    mitk::UnstructuredGrid::Pointer m_VolumeMesh;
    mitk::Image::Pointer m_IntensityImage;
    TFunctor m_LinearFunctor, m_PowerLawFunctor;
};