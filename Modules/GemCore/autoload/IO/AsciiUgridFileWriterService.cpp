#include "AsciiUgridFileWriterService.h"
#include <mitkUnstructuredGrid.h>
#include <mitkCustomMimeType.h>
#include <GemIOMimeTypes.h>
#include <itksys/SystemTools.hxx>
#include <vtkUnstructuredGrid.h>
#include <vtkPointData.h>
#include <boost/format.hpp>

namespace
{
    void serialize(std::ofstream &rFile, vtkUnstructuredGrid &rGrid)
    {
        auto pPointIDArray = rGrid.GetPointData()->GetArray("vtkOriginalPointIds");
        auto uiNumberOfPoints = rGrid.GetNumberOfPoints();

        for(auto i = 0; i<uiNumberOfPoints; i++)
        {
            const auto point = rGrid.GetPoint(i);
            uint32_t uiPointID;
            if(pPointIDArray != nullptr)
            {
                uiPointID = pPointIDArray->GetTuple1(i);
            }
            else
            {
                uiPointID = i + 1;
            }

            rFile << uiPointID << " "
                  << boost::format("%12.4f") % point[0] << ", "
                  << boost::format("%12.4f") % point[1] << ", "
                  << boost::format("%12.4f") % point[2] << ", ";
        }
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
    InputType::ConstPointer input = dynamic_cast<const InputType*>(this->GetInput());
    if (input.IsNull())
    {
        MITK_ERROR <<"Input is NULL!";
        return;
    }
    if (this->GetOutputLocation().empty())
    {
        MITK_ERROR << "Filename has not been set!" ;
        return ;
    }

    std::string ext = itksys::SystemTools::GetFilenameLastExtension(this->GetOutputLocation());
    ext = itksys::SystemTools::LowerCase(ext);

    // default extension is .txt
    if(ext == "")
    {
        ext = ".txt";
        this->SetOutputLocation(this->GetOutputLocation() + ext);
    }

    try
    {
        std::ofstream file(this->GetOutputLocation());

        if ( file.is_open() )
        {
            serialize(file, *const_cast<InputType&>(*input).GetVtkUnstructuredGrid()); // For whatever reason GetVtkUnstructuredGrid is not const...
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

AsciiUgridFileWriterService* AsciiUgridFileWriterService::Clone() const
{
    return new AsciiUgridFileWriterService(*this);
}