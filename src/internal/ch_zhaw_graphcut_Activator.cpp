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


#include "ch_zhaw_graphcut_Activator.h"

#include <QtPlugin>
#include <mitkWorkbenchUtil.h>
#include <ctkUtils.h>
#include <QApplication>
#include <berryPlatform.h>

#include "GraphcutView.h"

namespace mitk {

void ch_zhaw_graphcut_Activator::start(ctkPluginContext* context)
{
    BERRY_REGISTER_EXTENSION_CLASS(GraphcutView, context)

    // set the logo
    mitk::WorkbenchUtil::SetDepartmentLogoPreference(":/GraphCutPlugin/logo.png", context);
}

void ch_zhaw_graphcut_Activator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
}

}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(ch_zhaw_graphcut, mitk::ch_zhaw_graphcut_Activator)
#endif
