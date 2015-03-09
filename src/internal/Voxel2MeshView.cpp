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
#include "Voxel2MeshView.h"

// Qt
#include <QMessageBox>

//mitk image
#include <mitkImage.h>

const std::string Voxel2MeshView::VIEW_ID = "org.mitk.views.voxelmasktopolygonmesh";

void Voxel2MeshView::SetFocus() {
}

void Voxel2MeshView::CreateQtPartControl(QWidget * parent) {
    m_Controls.setupUi(parent);
}

void Voxel2MeshView::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/, const QList <mitk::DataNode::Pointer> &nodes) {
    if(nodes.count() == 0) {
        m_Controls.selectedImages->setText("no image selected");
        return;
    }
    else {
        QString selectedImageNames;

        bool firstImage = true;
        foreach(mitk::DataNode::Pointer node, nodes){
            if(!firstImage) {
                selectedImageNames.append(", ");
            } else {
                firstImage = false;
            }
            mitk::StringProperty* nameProperty= (mitk::StringProperty*)(node->GetProperty("name"));
            selectedImageNames.append(nameProperty->GetValue());
        }
        m_Controls.selectedImages->setText(selectedImageNames);
    }
}