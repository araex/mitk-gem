#pragma once

#include <ctkPluginActivator.h>

namespace mitk {
    class ch_zhaw_materialmapping_Activator : public QObject, public ctkPluginActivator {
        Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        Q_PLUGIN_METADATA(IID "ch_zhaw_materialmapping")
#endif
        Q_INTERFACES(ctkPluginActivator)

    public:
        void start(ctkPluginContext *context);
        void stop(ctkPluginContext *context);
    };
}
