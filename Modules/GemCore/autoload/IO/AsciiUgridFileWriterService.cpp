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

#include "GemIOResources.h"

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

        rFile << "#COMMENT Structure: node_number, x, y, z, TC, TD" << std::endl;
        rFile << "#COMMENT The TA – TE are the Young´s moduli at the nodes for method D and D respectively."
              << std::endl;
        rFile << "#BEGIN NODES" << std::endl;
        for (auto i = 0; i < uiNumberOfPoints; i++)
        {
            const auto point = rGrid.GetPoint(i);

            rFile << getPointID(i) << ", "
                  << boost::format("%12.4f") % point[0] << ", "
                  << boost::format("%12.4f") % point[1] << ", "
                  << boost::format("%12.4f") % point[2] << ", "
                  << boost::format("%12.4f") % getPointC(i) << ", "
                  << boost::format("%12.4f") % getPointD(i) << std::endl;
        }
        rFile << "#END NODES" << std::endl;

        rFile << "#COMMENT Structure: elem_nr, n1, n2, n3, , , , , n10, EA, EB, EE" << std::endl;
        rFile << "#COMMENT The EA, EB, EE are the Young´s moduli at the elements for method A, B and E respectively."
              << std::endl;
        rFile << "#BEGIN ELEMENTS" << std::endl;
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
                  << boost::format("%12.4f") % getCellB(i) << ", "
                  << boost::format("%12.4f") % getCellE(i) << std::endl;
        }
        rFile << "#END ELEMENTS" << std::endl;
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

        MITK_INFO << "Example Data Structure has been written";
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