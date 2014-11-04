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


#include <berryStarter.h>
#include <Poco/Util/MapConfiguration.h>

#include <QApplication>

int main(int argc, char** argv)
{
  // Create a QApplication instance first
  QApplication myApp(argc, argv);
  myApp.setApplicationName("MITK Workbench");
  myApp.setOrganizationName("ZHAW, Visual Computing Lab");

  // These paths replace the .ini file and are tailored for installation
  // packages created with CPack. If a .ini file is presented, it will
  // overwrite the settings in MapConfiguration
  Poco::Path basePath(argv[0]);
  basePath.setFileName("");

  Poco::Path provFile(basePath);
  provFile.setFileName("Workbench.provisioning");

  Poco::Util::MapConfiguration* sbConfig(new Poco::Util::MapConfiguration());
  sbConfig->setString(berry::Platform::ARG_PROVISIONING, provFile.toString());
  sbConfig->setString(berry::Platform::ARG_APPLICATION, "org.mitk.qt.extapplication");
  return berry::Starter::Run(argc, argv, sbConfig);
}
