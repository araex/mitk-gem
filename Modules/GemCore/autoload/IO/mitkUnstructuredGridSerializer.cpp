#include "mitkUnstructuredGridSerializer.h"
#include "mitkUnstructuredGrid.h"

#include <itksys/SystemTools.hxx>
#include <mitkIOUtil.h>


MITK_REGISTER_SERIALIZER(UnstructuredGridSerializer)

std::string mitk::UnstructuredGridSerializer::Serialize()
{
    const UnstructuredGrid *ug = dynamic_cast<const UnstructuredGrid *>( m_Data.GetPointer());
    if (ug == nullptr)
    {
        MITK_ERROR << " Object at " << (const void *) this->m_Data
                   << " is not an mitk::UnstructuredGrid. Cannot serialize as UnstructuredGrid.";
        return "";
    }

    std::string filename(this->GetUniqueFilenameInWorkingDirectory());
    filename += "_";
    filename += m_FilenameHint;
    filename += ".vtu";

    std::string fullname(m_WorkingDirectory);
    fullname += "/";
    fullname += itksys::SystemTools::ConvertToOutputPath(filename.c_str());

    try
    {
        mitk::IOUtil::Save(const_cast<UnstructuredGrid *>(ug), fullname);
    }
    catch (std::exception &e)
    {
        MITK_ERROR << " Error serializing object at " << (const void *) this->m_Data << " to " << fullname << ": "
                   << e.what();
        return "";
    }
    return filename;
}

