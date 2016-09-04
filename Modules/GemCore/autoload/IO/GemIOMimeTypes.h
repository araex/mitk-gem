#pragma once

#include "mitkCustomMimeType.h"

#include <string>

class GemIOMimeTypes {
public:
    static mitk::CustomMimeType ANSYS_MIMETYPE();
    static std::string ANSYS_MIMETYPE_NAME();

    static mitk::CustomMimeType ASCIIUGRID_MIMETYPE();
    static std::string ASCIIUGRID_MIMETYPE_NAME();

    // get all mime types used in mitk-gem
    static std::vector<mitk::CustomMimeType*> Get();
};
