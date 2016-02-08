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

#include "ch_zhaw_volumemesh_Activator.h"
#include <QtPlugin>
#include "VolumeMeshView.h"

namespace mitk {
    void ch_zhaw_volumemesh_Activator::start(ctkPluginContext *context) {
        BERRY_REGISTER_EXTENSION_CLASS(VolumeMeshView, context)
    }

    void ch_zhaw_volumemesh_Activator::stop(ctkPluginContext *context) {
        Q_UNUSED(context)
    }
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(ch_zhaw_volumemesh, mitk::ch_zhaw_volumemesh_Activator)
#endif
