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


#ifndef ch_eth_gemio_Activator_h
#define ch_eth_gemio_Activator_h

#include <ctkPluginActivator.h>
#include "AnsysFileWriterService.h"

namespace mitk {

class ch_eth_gemio_Activator :
  public QObject, public ctkPluginActivator
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "ch_eth_gemio")
  Q_INTERFACES(ctkPluginActivator)

public:

  void start(ctkPluginContext* context);
  void stop(ctkPluginContext* context);

private:
    std::unique_ptr<AnsysFileWriterService> spAnsysFileWriterInstance;

}; // ch_eth_gemio_Activator

}

#endif // ch_eth_gemio_Activator_h
