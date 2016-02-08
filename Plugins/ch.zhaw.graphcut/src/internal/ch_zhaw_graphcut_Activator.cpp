/**
 *  MITK-GEM: Graphcut Plugin
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#include "ch_zhaw_graphcut_Activator.h"

#include <QtPlugin>
#include <ctkUtils.h>
#include <QApplication>
#include <berryPlatform.h>

#include "GraphcutView.h"

namespace mitk {

void ch_zhaw_graphcut_Activator::start(ctkPluginContext* context)
{
    BERRY_REGISTER_EXTENSION_CLASS(GraphcutView, context)
}

void ch_zhaw_graphcut_Activator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context)
}

}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(ch_zhaw_graphcut, mitk::ch_zhaw_graphcut_Activator)
#endif
