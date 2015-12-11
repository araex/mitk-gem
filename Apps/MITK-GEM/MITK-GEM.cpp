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


#include <mitkBaseApplication.h>

#include <QVariant>

int main(int argc, char** argv)
{
  // Create a QApplication instance first
  mitk::BaseApplication myApp(argc, argv);
  myApp.setApplicationName("MITK-GEM");
  myApp.setOrganizationName("ZHAW");

  myApp.setProperty(mitk::BaseApplication::PROP_APPLICATION, "org.mitk.qt.extapplication");
  return myApp.run();
}
