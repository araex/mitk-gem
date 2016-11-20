#include "GemIOMimeTypes.h"
#include "mitkIOMimeTypes.h"
#include <mitkLogMacros.h>

std::vector<mitk::CustomMimeType *> GemIOMimeTypes::Get() {
    std::vector <mitk::CustomMimeType *> mimeTypes;

    // order matters here (descending rank for mime types)
    mimeTypes.push_back(ANSYS_MIMETYPE().Clone());

    return mimeTypes;
}

mitk::CustomMimeType GemIOMimeTypes::ANSYS_MIMETYPE(void)
{
    static std::string name(ANSYS_MIMETYPE_NAME());
    mitk::CustomMimeType mimeType(name);
    mimeType.SetComment("ANSYS unstructured grid data");
    mimeType.SetCategory("GEM Unstructured Grid");
    mimeType.AddExtension("inp");
    return mimeType;
}

std::string GemIOMimeTypes::ANSYS_MIMETYPE_NAME() {
    // create a unique and sensible name for this mime type
    static std::string name(mitk::IOMimeTypes::DEFAULT_BASE_NAME() + ".gem.ugridansys");
    return name;
}

mitk::CustomMimeType GemIOMimeTypes::ASCIIUGRID_MIMETYPE(void)
{
    static std::string name(ASCIIUGRID_MIMETYPE_NAME());
    mitk::CustomMimeType mimeType(name);
    mimeType.SetComment("ASCII unstructured grid data");
    mimeType.SetCategory("GEM Unstructured Grid");
    mimeType.AddExtension("txt");
    return mimeType;
}

std::string GemIOMimeTypes::ASCIIUGRID_MIMETYPE_NAME() {
    // create a unique and sensible name for this mime type
    static std::string name(mitk::IOMimeTypes::DEFAULT_BASE_NAME() + ".gem.ugridascii");
    return name;
}