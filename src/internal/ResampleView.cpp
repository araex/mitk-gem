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


// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "ResampleView.h"

// Qt
#include <QMessageBox>

//mitk image
#include <mitkImage.h>

const std::string ResampleView::VIEW_ID = "org.mitk.views.resampleview";

void ResampleView::SetFocus() {}

void ResampleView::CreateQtPartControl( QWidget *parent ) {
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi( parent );
}

void ResampleView::OnSelectionChanged( berry::IWorkbenchPart::Pointer /*source*/, const QList<mitk::DataNode::Pointer>& nodes ) {}


void ResampleView::ApplyButtonPressed() {

}
