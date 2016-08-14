#include "AnsysFileWriterService.h"
#include <mitkUnstructuredGrid.h>
#include <mitkCustomMimeType.h>
#include <GemIOMimeTypes.h>

AnsysFileWriterService::AnsysFileWriterService(void)
        : mitk::AbstractFileWriter(mitk::UnstructuredGrid::GetStaticNameOfClass(),
                                   GemIOMimeTypes::ANSYS_MIMETYPE(),
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