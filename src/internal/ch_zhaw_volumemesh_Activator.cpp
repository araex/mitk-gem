/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/


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
