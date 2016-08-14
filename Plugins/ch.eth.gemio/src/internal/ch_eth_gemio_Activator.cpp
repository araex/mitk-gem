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

#include "ch_eth_gemio_Activator.h"
#include "GemIOView.h"

namespace mitk {


void ch_eth_gemio_Activator::start(ctkPluginContext* context)
{
  BERRY_REGISTER_EXTENSION_CLASS(GemIOView, context);
  spAnsysFileWriterInstance = std::unique_ptr<AnsysFileWriterService>(new AnsysFileWriterService());
}

void ch_eth_gemio_Activator::stop(ctkPluginContext* context)
{
  Q_UNUSED(context);
  spAnsysFileWriterInstance.reset();
}

}
