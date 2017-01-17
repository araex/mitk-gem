#pragma once

#include "mitkBaseDataSerializer.h"

namespace mitk
{
    /*!
     * SceneIO Serializer for mitk::UnstructuredGrid
     * @author Thomas Fitze
     */
    class UnstructuredGridSerializer : public BaseDataSerializer
    {
    public:
        mitkClassMacro( UnstructuredGridSerializer, BaseDataSerializer);
        itkFactorylessNewMacro(Self)
        itkCloneMacro(Self)
        virtual std::string Serialize() override;
    protected:
        UnstructuredGridSerializer() = default;
        virtual ~UnstructuredGridSerializer() = default;
    };
}
