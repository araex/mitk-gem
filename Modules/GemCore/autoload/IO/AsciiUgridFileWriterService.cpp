#include "AsciiUgridFileWriterService.h"
#include <mitkUnstructuredGrid.h>
#include <mitkCustomMimeType.h>
#include <GemIOMimeTypes.h>
#include <itksys/SystemTools.hxx>
#include <vtkUnstructuredGrid.h>
#include <vtkPointData.h>
#include <boost/format.hpp>
#include <vtkType.h>
#include <vtkCellData.h>
#include <vtkCell.h>
#include <vtkCellTypes.h>
#include <vtkUnstructuredGridBase.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGridGeometryFilter.h>
#include <vtkIdTypeArray.h>

#include "GemIOResources.h"

using VtkUGrid = vtkSmartPointer<vtkUnstructuredGridBase>;

namespace
{
    template<class TValue = float, class TArray>
    std::function<TValue(vtkIdType)> createArrayAccessFunctor(TArray *p, TValue defaultValue = 0)
    {
        if (p != nullptr)
        {
            return [p](vtkIdType id)
            { return p->GetTuple1(id); };
        }
        else
        {
            MITK_INFO("AsciiUgridFileWriterService") << "No Array found. Using default value " << defaultValue;
            return [defaultValue](vtkIdType)
            { return defaultValue; };;
        }
    };

    template<class TArray>
    std::function<vtkIdType(vtkIdType)> createIDFunctor(TArray *p)
    {
        if (p != nullptr)
        {
            return [p](vtkIdType id)
            { return p->GetTuple1(id); };
        }
        else
        {
            MITK_INFO("AsciiUgridFileWriterService") << "No VTK IDs found.";
            return [](vtkIdType i)
            { return i + 1; };
        }
    }

    VtkUGrid extractSurface(const VtkUGrid _volMesh)
    {
        auto surfaceFilter = vtkSmartPointer<vtkUnstructuredGridGeometryFilter>::New();
        surfaceFilter->SetInputData(_volMesh);
        surfaceFilter->MergingOff();
        surfaceFilter->PassThroughCellIdsOn();
        surfaceFilter->PassThroughPointIdsOn();
        surfaceFilter->SetOriginalCellIdsName("vtkOriginalCellIds");
        surfaceFilter->SetOriginalPointIdsName("vtkOriginalPointIds");
        surfaceFilter->Update();
        return surfaceFilter->GetOutput();
    }

    void extractAndSerializeSurface(std::ofstream &rFile, VtkUGrid spMesh)
    {
        auto spSurface = extractSurface(spMesh);

        vtkIdTypeArray* pPointIDs = vtkIdTypeArray::SafeDownCast(spSurface->GetPointData()->GetArray("vtkOriginalPointIds"));
        vtkIdTypeArray* pCellIDs = vtkIdTypeArray::SafeDownCast(spSurface->GetCellData()->GetArray("vtkOriginalCellIds"));

        auto uiNumberOfPoints = spSurface->GetNumberOfPoints();
        auto uiNumberOfCells = spSurface->GetNumberOfCells();

        MITK_INFO("AsciiUgridFileWriterService") << "Writing Surface: " << uiNumberOfPoints << " nodes, " << uiNumberOfCells << "cells. ";

        rFile << "#BEGIN SURFACE" << std::endl;
        rFile << "#COMMENT Structure: element_number, n1, n2, n3, n4, n5, n6" << std::endl;
        for(auto i = 0; i < uiNumberOfCells; ++i)
        {
            const auto pCell = spSurface->GetCell(i);

            rFile << pCellIDs->GetValue(i) << ", ";
            int32_t iNumberOfPoints = pCell->GetNumberOfPoints();
            for (auto j = 0; j < iNumberOfPoints; ++j)
            {
                auto pointId = pPointIDs->GetValue(pCell->GetPointId(j));
                rFile << pointId << (j == iNumberOfPoints-1 ? "" : ", ");
            }
            rFile << std::endl;
        }
        rFile << "#END SURFACE" << std::endl;
    }

    void serialize(std::ofstream &rFile, vtkUnstructuredGrid &rGrid)
    {
        // vtkCellData
        auto getPointID = createIDFunctor(rGrid.GetPointData()->GetArray("vtkOriginalPointIds"));
        auto getPointC = createArrayAccessFunctor(rGrid.GetPointData()->GetArray(GEM_DATA_ARRAY_NAME_MATMAP_METHOD_C));
        auto getPointD = createArrayAccessFunctor(rGrid.GetPointData()->GetArray(GEM_DATA_ARRAY_NAME_MATMAP_METHOD_D));

        auto getCellID = createIDFunctor(rGrid.GetCellData()->GetArray("vtkOriginalCellIds"));
        auto getCellA = createArrayAccessFunctor(rGrid.GetCellData()->GetArray(GEM_DATA_ARRAY_NAME_MATMAP_METHOD_A));
        auto getCellB = createArrayAccessFunctor(rGrid.GetCellData()->GetArray(GEM_DATA_ARRAY_NAME_MATMAP_METHOD_B));
        auto getCellE = createArrayAccessFunctor(rGrid.GetCellData()->GetArray(GEM_DATA_ARRAY_NAME_MATMAP_METHOD_E));

        auto uiNumberOfPoints = rGrid.GetNumberOfPoints();
        auto uiNumberOfCells = rGrid.GetNumberOfCells();

        MITK_INFO("AsciiUgridFileWriterService") << "Writing mesh: " << uiNumberOfPoints << " nodes, " << uiNumberOfCells << "cells. ";

        rFile << "#COMMENT Structure: node_number, x, y, z, TC" << std::endl;
        rFile << "#COMMENT TC is the Young´s moduli at the nodes for method C."
              << std::endl;
        rFile << "#BEGIN NODES" << std::endl;
        for (auto i = 0; i < uiNumberOfPoints; i++)
        {
            const auto point = rGrid.GetPoint(i);

            rFile << getPointID(i) << ", "
                  << boost::format("%12.4f") % point[0] << ", "
                  << boost::format("%12.4f") % point[1] << ", "
                  << boost::format("%12.4f") % point[2] << ", "
                  << boost::format("%12.4f") % getPointC(i) << std::endl;
        }
        rFile << "#END NODES" << std::endl;

        vtkSmartPointer<vtkCellTypes> cellTypes = vtkCellTypes::New();
        rGrid.GetCellTypes(cellTypes);
        uint32_t uiPointsPerCell = 0;
        if(cellTypes->IsType(VTK_TETRA))
        {
            uiPointsPerCell = 4;
            MITK_INFO("AsciiUgridFileWriterService") << "Cell Type: VTK_TETRA";
        }
        else if(cellTypes->IsType(VTK_QUADRATIC_TETRA))
        {
            uiPointsPerCell = 10;
            MITK_INFO("AsciiUgridFileWriterService") << "Cell Type: VTK_QUADRATIC_TETRA";
        }
        else
        {
            MITK_WARN("AsciiUgridFileWriterService") << "Unknown cell type";
        }
        rFile << "#COMMENT Structure: elem_nr, n1, ... , n" << uiPointsPerCell << ", EA, EB" << std::endl;
        rFile << "#COMMENT EA, EB are the Young´s moduli at the elements for method A and B respectively."
              << std::endl;
        rFile << "#BEGIN ELEMENTS "<< uiPointsPerCell << std::endl;
        for (auto i = 0; i < uiNumberOfCells; ++i)
        {
            const auto pCell = rGrid.GetCell(i);

            rFile << getCellID(i) << ", ";
            for (auto j = 0; j < pCell->GetNumberOfPoints(); ++j)
            {
                auto pointId = pCell->GetPointId(j); // +1 ??
                rFile << pointId << ", ";
            }

            rFile << boost::format("%12.4f") % getCellA(i) << ", "
                  << boost::format("%12.4f") % getCellB(i) << std::endl;
        }
        rFile << "#END ELEMENTS " << uiPointsPerCell << std::endl;

        extractAndSerializeSurface(rFile, &rGrid);
    }
}

AsciiUgridFileWriterService::AsciiUgridFileWriterService(void)
        : mitk::AbstractFileWriter(mitk::UnstructuredGrid::GetStaticNameOfClass(),
                                   GemIOMimeTypes::ASCIIUGRID_MIMETYPE(),
                                   "ASCIIugrid")
{
    RegisterService();
}

AsciiUgridFileWriterService::AsciiUgridFileWriterService(const AsciiUgridFileWriterService &other)
        : mitk::AbstractFileWriter(other)
{

}

AsciiUgridFileWriterService::~AsciiUgridFileWriterService()
{

}

void AsciiUgridFileWriterService::Write()
{
    // TODO
    MITK_INFO << "Writing AsciiUgrid.";

    using InputType = mitk::UnstructuredGrid;
    InputType::ConstPointer input = dynamic_cast<const InputType *>(this->GetInput());
    if (input.IsNull())
    {
        MITK_ERROR << "Input is NULL!";
        return;
    }
    if (this->GetOutputLocation().empty())
    {
        MITK_ERROR << "Filename has not been set!";
        return;
    }

    std::string ext = itksys::SystemTools::GetFilenameLastExtension(this->GetOutputLocation());
    ext = itksys::SystemTools::LowerCase(ext);

    // default extension is .txt
    if (ext == "")
    {
        ext = ".txt";
        this->SetOutputLocation(this->GetOutputLocation() + ext);
    }

    try
    {
        std::ofstream file(this->GetOutputLocation());

        if (file.is_open())
        {
            serialize(file,
                      *const_cast<InputType &>(*input).GetVtkUnstructuredGrid()); // For whatever reason GetVtkUnstructuredGrid is not const...
        }
        else
        {
            mitkThrow() << "Could not open file " << this->GetOutputLocation() << " for writing.";
        }

        MITK_INFO << "Mesh exported";
    }

    catch (mitk::Exception e)
    {
        MITK_ERROR << e.GetDescription();
    }
    catch (...)
    {
        MITK_ERROR << "Unknown error occurred while trying to write file.";
    }
}

AsciiUgridFileWriterService *AsciiUgridFileWriterService::Clone() const
{
    return new AsciiUgridFileWriterService(*this);
}