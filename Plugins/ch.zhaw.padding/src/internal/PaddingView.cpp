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
#include "PaddingView.h"

// Qt
#include <QMessageBox>


const std::string PaddingView::VIEW_ID = "org.mitk.views.paddingview";

void PaddingView::CreateQtPartControl(QWidget *parent) {
    // create GUI widgets from the Qt Designer's .ui file
    m_Controls.setupUi(parent);

    // setup padding buttons
    connect(m_Controls.padLeft, SIGNAL(clicked()), this, SLOT(padLeftButtonPressed()));
    connect(m_Controls.padUp, SIGNAL(clicked()), this, SLOT(padUpButtonPressed()));
    connect(m_Controls.padRight, SIGNAL(clicked()), this, SLOT(padRightButtonPressed()));
    connect(m_Controls.padDown, SIGNAL(clicked()), this, SLOT(padDownButtonPressed()));
}

void PaddingView::SetFocus() {}

void PaddingView::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/, const QList <mitk::DataNode::Pointer> &nodes) {

    if(nodes.count() == 0){
        m_Controls.affectedImages->setText("no image selected");
        return;
    }
    else{
        QString selectedImageNames;

        bool firstImage = true;
        foreach(mitk::DataNode::Pointer
        node, nodes){
            if(!firstImage) {
                selectedImageNames.append(", ");
            } else{
                firstImage = false;
            }
            mitk::StringProperty* nameProperty= (mitk::StringProperty*)(node->GetProperty("name"));
            selectedImageNames.append(nameProperty->GetValue());
        }
        m_Controls.affectedImages->setText(selectedImageNames);
    }

}

void PaddingView::padLeftButtonPressed() {
    WorkbenchUtils::Axis axis = (WorkbenchUtils::Axis) m_Controls.axisComboBox->currentIndex();

    switch (axis) {
        case WorkbenchUtils::Axis::AXIAL:
            addPadding(WorkbenchUtils::Axis::SAGITTAL, false);
            return;
        case WorkbenchUtils::Axis::SAGITTAL:
            addPadding(WorkbenchUtils::Axis::CORONAL, false);
        case WorkbenchUtils::Axis::CORONAL:
            addPadding(WorkbenchUtils::Axis::SAGITTAL, false);
            return;
    }
}

void PaddingView::padRightButtonPressed() {
    WorkbenchUtils::Axis axis = (WorkbenchUtils::Axis) m_Controls.axisComboBox->currentIndex();

    switch (axis) {
        case WorkbenchUtils::Axis::AXIAL:
            addPadding(WorkbenchUtils::Axis::SAGITTAL, true);
            return;
        case WorkbenchUtils::Axis::SAGITTAL:
            addPadding(WorkbenchUtils::Axis::CORONAL, true);
        case WorkbenchUtils::Axis::CORONAL:
            addPadding(WorkbenchUtils::Axis::SAGITTAL, true);
            return;
    }

}

void PaddingView::padUpButtonPressed() {
    WorkbenchUtils::Axis axis = (WorkbenchUtils::Axis) m_Controls.axisComboBox->currentIndex();

    switch (axis) {
        case WorkbenchUtils::Axis::AXIAL:
            addPadding(WorkbenchUtils::Axis::CORONAL, false);
            return;
        case WorkbenchUtils::Axis::SAGITTAL:
            addPadding(WorkbenchUtils::Axis::AXIAL, true);
        case WorkbenchUtils::Axis::CORONAL:
            addPadding(WorkbenchUtils::Axis::AXIAL, true);
            return;
    }
}

void PaddingView::padDownButtonPressed() {
    WorkbenchUtils::Axis axis = (WorkbenchUtils::Axis) m_Controls.axisComboBox->currentIndex();

    switch (axis) {
        case WorkbenchUtils::Axis::AXIAL:
            addPadding(WorkbenchUtils::Axis::CORONAL, true);
            return;
        case WorkbenchUtils::Axis::SAGITTAL:
            addPadding(WorkbenchUtils::Axis::AXIAL, false);
        case WorkbenchUtils::Axis::CORONAL:
            addPadding(WorkbenchUtils::Axis::AXIAL, false);
            return;
    }
}

void PaddingView::addPadding(WorkbenchUtils::Axis axis, bool append) {
    // get nodes
    QList <mitk::DataNode::Pointer> nodes = this->GetDataManagerSelection();

    // get params
    float voxelValue = m_Controls.voxelValueSpinBox->value();
    unsigned int amountOfPadding = m_Controls.amountOfPaddingSpinBox->value();

    foreach(mitk::DataNode::Pointer
    node, nodes){
        mitk::Image::Pointer img = dynamic_cast<mitk::Image *>(node->GetData());
        img = WorkbenchUtils::addPadding(img, axis, append, amountOfPadding, voxelValue);
        node->SetData(img);
    }

    refreshBoundaries();
}

void PaddingView::refreshBoundaries() {
    mitk::RenderingManager::GetInstance()->InitializeViewsByBoundingObjects(this->GetDataStorage());
}
