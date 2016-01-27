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

#include "QmitkExtAppWorkbenchAdvisor.h"
#include "internal/QmitkExtApplicationPlugin.h"

#include <QmitkExtWorkbenchWindowAdvisor.h>

const QString QmitkExtAppWorkbenchAdvisor::DEFAULT_PERSPECTIVE_ID = "org.mitk.perspectives.gem";

void
QmitkExtAppWorkbenchAdvisor::Initialize(berry::IWorkbenchConfigurer::Pointer configurer)
{
  berry::QtWorkbenchAdvisor::Initialize(configurer);
  configurer->SetSaveAndRestore(true);
}

berry::WorkbenchWindowAdvisor*
QmitkExtAppWorkbenchAdvisor::CreateWorkbenchWindowAdvisor(berry::IWorkbenchWindowConfigurer::Pointer configurer)
{
  QmitkExtWorkbenchWindowAdvisor* advisor = new QmitkExtWorkbenchWindowAdvisor(this, configurer);

  // Exclude the help perspective from org.blueberry.ui.qt.help from
  // the normal perspective list.
  // The perspective gets a dedicated menu entry in the help menu
  QList<QString> excludePerspectives;
  excludePerspectives.push_back("org.blueberry.perspectives.help");
  excludePerspectives.push_back("org.mitk.mitkworkbench.perspectives.editor");
  excludePerspectives.push_back("org.mitk.mitkworkbench.perspectives.visualization");
  advisor->SetPerspectiveExcludeList(excludePerspectives);

  // Exclude some views from the normal view list
  QList<QString> excludeViews;
  excludeViews.push_back("org.mitk.views.modules");
  excludeViews.push_back("org.blueberry.views.helpindex");
  excludeViews.push_back("org.blueberry.views.helpsearch");
  excludeViews.push_back("org.mitk.views.deformableclippingplane");
  excludeViews.push_back("org.mitk.views.datamanager");
  advisor->SetViewExcludeList(excludeViews);

  // general settings
  advisor->SetWindowIcon(":/ch.zhaw.gemapplication/icon.png");
  advisor->SetProductName("MITK-GEM v2016.2-beta.1");
  advisor->ShowVersionInfo(false);
  advisor->ShowMitkVersionInfo(false);
  return advisor;
}

QString QmitkExtAppWorkbenchAdvisor::GetInitialWindowPerspectiveId()
{
  return DEFAULT_PERSPECTIVE_ID;
}
