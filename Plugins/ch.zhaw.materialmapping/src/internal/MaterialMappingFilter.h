#pragma once

#include <string>

#include <mitkImage.h>
#include <mitkUnstructuredGridToUnstructuredGridFilter.h>

#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkImageStencil.h>


#include "BoneDensityFunctor.h"
#include "PowerLawFunctor.h"

/**
 * Given the input:
 * - CT image
 * - Volume mesh (unstructured grid)
 * - Bone Density Functor (HU->gHA/cmˆ3)
 * - Power Law Functor
 * This filter outputs a material mapped mesh.
 *
 *  1. Creates a working copy of the CT image
 *  2. Casts the working copy value type to float
 *  3. Extracts a surface out of the unstructured grid (ugrid)
 *  4. Extracts a volume of interest (VOI) defined by the axis aligned bounding box of the surface + padding
 *  5. Evaluates the given functors for each voxel in the VOI
 *  6. Get a stencil from the surface
 *  7. (configurable) peel step.
 *  8. (configurable) image extends.
 *  9. Interpolate functor results to mesh nodes (=points)
 * 10. Calculate element (=cell) values by averaging surrounding node values.
 * 11. Add point and cell data (both named "E") to the output mesh.
 * 12. Return mesh
 *
 * Note that 2 different mapping methods are available:
 * - The "old" or current one. This is the approach discussed in the paper.
 * - A newer one containing some improvements for more accurate results that have yet to be verified.
 *
 * The mapping is functionally equivalent to assignElasticModulus.cc 26.11.15 (v3).
 */
class MaterialMappingFilter : public mitk::UnstructuredGridToUnstructuredGridFilter
{
public:
	enum class Method
	{
		Old, // as originally published
		New // modified and improved
	};

	mitkClassMacro(MaterialMappingFilter, UnstructuredGridToUnstructuredGridFilter)

	itkFactorylessNewMacro(Self)

	itkCloneMacro(Self)

	void SetMethod(Method _m)
	{
		m_Method = _m;
	}

	void SetIntensityImage(mitk::Image::Pointer _p)
	{
		m_IntensityImage = _p;
	}

	void SetDensityFunctor(BoneDensityFunctor&& _f)
	{
		m_BoneDensityFunctor = _f;
	}

	void SetPowerLawFunctor(PowerLawFunctor&& _f)
	{
		m_PowerLawFunctor = _f;
	}

	void SetDoPeelStep(bool _b)
	{
		m_DoPeelStep = _b;
	}

	void SetNumberOfExtendImageSteps(unsigned int _i)
	{
		m_NumberOfExtendImageSteps = _i;
	}

	void SetMinElementValue(float _f)
	{
		m_MinimumElementValue = _f;
	}

	void SetIntermediateResultOutputDirectory(std::string _d)
	{
		m_VerboseOutput = true;
		m_VerboseOutputDirectory = _d;
	}

	virtual void GenerateData() override;

protected:
	using VtkImage = vtkSmartPointer<vtkImageData>;
	using VtkStencil = vtkSmartPointer<vtkImageStencil>;
	using VtkUGrid = vtkSmartPointer<vtkUnstructuredGridBase>;
	using VtkDoubleArray = vtkSmartPointer<vtkDoubleArray>;

	MaterialMappingFilter();

	virtual ~MaterialMappingFilter()
	{
	};

	VtkUGrid extractSurface(const VtkUGrid) const;
	VtkImage extractVOI(const VtkImage, const VtkUGrid) const;
	VtkImage createStencil(const VtkUGrid, const VtkImage) const;
	VtkImage createPeeledMask(const VtkImage _img, const VtkImage _mask);
	void inplaceExtendImage(VtkImage _img, VtkImage _mask, bool _maxVal); // weighted average in neighborhood, performed in place
	void inplaceExtendImageOld(VtkImage _img, VtkImage _mask, bool _maxVal);
	void inplaceApplyFunctorsToImage(VtkImage _img);
	VtkDoubleArray interpolateToNodes(const VtkUGrid, const VtkImage, std::string _name, double _minElem) const; // "interpolateToNodes". evaluates both functors for each vertex of the mesh
	VtkDoubleArray nodesToElements(const VtkUGrid, VtkDoubleArray _nodeData, std::string _name) const;

	mitk::Image::Pointer m_IntensityImage;
	BoneDensityFunctor m_BoneDensityFunctor;
	PowerLawFunctor m_PowerLawFunctor;
	bool m_DoPeelStep = true, m_VerboseOutput;
	std::string m_VerboseOutputDirectory;
	float m_MinimumElementValue = 0.0;
	unsigned int m_NumberOfExtendImageSteps = 3;
	Method m_Method;

	void writeMetaImageToVerboseOut(const std::string filename, vtkSmartPointer<vtkImageData> image);
};
