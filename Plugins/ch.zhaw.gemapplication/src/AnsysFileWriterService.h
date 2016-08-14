#pragma once

#include <mitkAbstractFileWriter.h>

class AnsysFileWriterService : public mitk::AbstractFileWriter
{
public:
    AnsysFileWriterService(void);
    virtual ~AnsysFileWriterService(void);

    using mitk::AbstractFileWriter::Write;
    virtual void Write(void) override;

private:
    AnsysFileWriterService(const AnsysFileWriterService &other);
    virtual AnsysFileWriterService* Clone() const override;

    static mitk::CustomMimeType GetMimeType(void);
};