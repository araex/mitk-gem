#include <usModuleActivator.h>
#include <usModuleContext.h>

#include "GemIOMimeTypes.h"
#include <AnsysFileWriterService.h>
#include <AsciiUgridFileWriterService.h>

namespace mitk {
    class NewModuleIOActivator : public us::ModuleActivator {
    public:

        void Load(us::ModuleContext *context) override {
            // We can register our read/write services with a custom service ranking
            // services with a higher ranking are prioritized, default us 0
            us::ServiceProperties props;
            props[us::ServiceConstants::SERVICE_RANKING()] = 10;

            m_MimeTypes = GemIOMimeTypes::Get();
            for (std::vector<mitk::CustomMimeType *>::const_iterator mimeTypeIter = m_MimeTypes.begin(),
                         iterEnd = m_MimeTypes.end(); mimeTypeIter != iterEnd; ++mimeTypeIter) {
                context->RegisterService(*mimeTypeIter, props);
            }

            m_spAnsysFileWriterInstance = std::unique_ptr<AnsysFileWriterService>(new AnsysFileWriterService());
            m_spAsciiUgridFileWriterInstance = std::unique_ptr<AsciiUgridFileWriterService>(new AsciiUgridFileWriterService());
        }

        void Unload(us::ModuleContext *) override {
            for (unsigned int loop(0); loop < m_MimeTypes.size(); ++loop) {
                delete m_MimeTypes.at(loop);
            }

            m_spAnsysFileWriterInstance.reset();
            m_spAsciiUgridFileWriterInstance.reset();
        }

    private:
        std::unique_ptr <AnsysFileWriterService> m_spAnsysFileWriterInstance;
        std::unique_ptr <AsciiUgridFileWriterService> m_spAsciiUgridFileWriterInstance;

        std::vector<mitk::CustomMimeType *> m_MimeTypes;

    };
}

US_EXPORT_MODULE_ACTIVATOR(mitk::NewModuleIOActivator)
