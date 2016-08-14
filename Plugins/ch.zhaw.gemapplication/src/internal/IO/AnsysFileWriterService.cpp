#include "AnsysFileWriterService.h"
#include <mitkUnstructuredGrid.h>
#include <mitkCustomMimeType.h>
#include <mitkIOMimeTypes.h>

AnsysFileWriterService::AnsysFileWriterService(void)
        : mitk::AbstractFileWriter(mitk::UnstructuredGrid::GetStaticNameOfClass(),
                                   GetMimeType(),
                                   "ANSYS unstructured grid writer")
{
    RegisterService();
}

AnsysFileWriterService::AnsysFileWriterService(const AnsysFileWriterService &other)
: mitk::AbstractFileWriter(other)
{

}

AnsysFileWriterService::~AnsysFileWriterService()
{

}

void AnsysFileWriterService::Write()
{
    // TODO
    MITK_WARN("ch.eth.gemio") << "Write ugrid ";
}

AnsysFileWriterService* AnsysFileWriterService::Clone() const
{
    return new AnsysFileWriterService(*this);
}

mitk::CustomMimeType AnsysFileWriterService::GetMimeType(void)
{
    static std::string name(mitk::IOMimeTypes::DEFAULT_BASE_NAME() + ".mitk-gem.ansys_ugrid");
    mitk::CustomMimeType mimeType(name);
    mimeType.SetComment("ANSYS unstructured grid data");
    mimeType.SetCategory("Unstructured Grid");
    mimeType.AddExtension("kfile");
    return mimeType;
}