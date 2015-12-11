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


#ifndef ch_zhaw_materialmapping_Activator_h
#define ch_zhaw_materialmapping_Activator_h

#include <ctkPluginActivator.h>

namespace mitk {

    class ch_zhaw_materialmapping_Activator :
            public QObject, public ctkPluginActivator {
        Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        Q_PLUGIN_METADATA(IID "ch_zhaw_materialmapping")
#endif
                Q_INTERFACES(ctkPluginActivator)

    public:

        void start(ctkPluginContext *context);

        void stop(ctkPluginContext *context);

    }; // ch_zhaw_materialmapping_Activator

}

#endif // ch_zhaw_materialmapping_Activator_h
