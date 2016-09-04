#pragma once

#include <mitkAbstractFileWriter.h>

class AsciiUgridFileWriterService : public mitk::AbstractFileWriter
{
public:
    AsciiUgridFileWriterService(void);
    virtual ~AsciiUgridFileWriterService(void);

    using mitk::AbstractFileWriter::Write;
    virtual void Write(void) override;

private:
    AsciiUgridFileWriterService(const AsciiUgridFileWriterService &other);
    virtual AsciiUgridFileWriterService* Clone() const override;

    us::ServiceRegistration<mitk::IFileWriter> m_ServiceReg;
};