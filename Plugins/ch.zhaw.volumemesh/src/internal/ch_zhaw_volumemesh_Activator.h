/**
 *  MITK-GEM: Volume Mesher Plugin
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *  Copyright (c) 2016, ETH Zurich, Institute for Biomechanics, B. Helgason
 *  Copyright (c) 2016, University of Iceland, Mechanical Engineering and Computer Science, H. Pállson
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#pragma once

#include <ctkPluginActivator.h>

namespace mitk {
    class ch_zhaw_volumemesh_Activator :
            public QObject, public ctkPluginActivator {
        Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        Q_PLUGIN_METADATA(IID "ch_zhaw_volumemesh")
#endif
                Q_INTERFACES(ctkPluginActivator)
    public:
        void start(ctkPluginContext *context);
        void stop(ctkPluginContext *context);
    };
}
