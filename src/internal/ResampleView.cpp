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

#include "lib/WorkbenchUtils/WorkbenchUtils.h"

const std::string ResampleView::VIEW_ID = "org.mitk.views.resampleview";

void ResampleView::SetFocus() {
}

void ResampleView::CreateQtPartControl(QWidget * parent) {
    // create GUI widgets from the Qt Designer's .ui file
    m_Controls.setupUi(parent);

    // setup buttons
    connect(m_Controls.resampleButton, SIGNAL(clicked()), this, SLOT(resampleButtonPressed()));
    connect(m_Controls.copyButton, SIGNAL(clicked()), this, SLOT(copyButtonPressed()));
}

void ResampleView::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/, const QList <mitk::DataNode::Pointer> &nodes) {
    if (nodes.count() == 0) {
        m_Controls.selectedImage->setText("no image selected");
        setOriginDimension(0, 0, 0);
        return;
    } else if (nodes.count() == 1) {
        mitk::DataNode::Pointer node = nodes.at(0);
        mitk::StringProperty *nameProperty = (mitk::StringProperty *)(node->GetProperty("name"));
        m_Controls.selectedImage->setText(nameProperty->GetValue());
        updateDimensions(WorkbenchUtils::getImageByDataIndex(nodes, 0));
    } else {
        m_Controls.selectedImage->setText("invalid selection");
        setOriginDimension(0, 0, 0);
    }
}


void ResampleView::resampleButtonPressed() {
    // get nodes
    QList <mitk::DataNode::Pointer> nodes = this->GetDataManagerSelection();

    // check if the selection is valid
    if (nodes.count() == 1){
        // gather input
        mitk::Image::Pointer img = WorkbenchUtils::getImageByDataIndex(nodes, 0);
        unsigned int oldDimension[3];
        oldDimension[0] = img->GetDimension(0);
        oldDimension[1] = img->GetDimension(1);
        oldDimension[2] = img->GetDimension(2);
        unsigned int newDimension[3];
        newDimension[0] = m_Controls.resampleDim1->value();
        newDimension[1] = m_Controls.resampleDim2->value();
        newDimension[2] = m_Controls.resampleDim3->value();

    }
    // TODO: handle invalid selections
}

void ResampleView::copyButtonPressed() {
    unsigned int dim1 = m_Controls.originDim1->value();
    unsigned int dim2 = m_Controls.originDim2->value();
    unsigned int dim3 = m_Controls.originDim3->value();
    setResampleDimension(dim1, dim2, dim3);
}

void ResampleView::updateDimensions(mitk::Image::Pointer img) {
    if(img){
        unsigned int dim1 = img->GetDimension(0);
        unsigned int dim2 = img->GetDimension(1);
        unsigned int dim3 = img->GetDimension(2);
        setOriginDimension(dim1, dim2, dim3);
    } else{
        setOriginDimension(0, 0, 0);
    }
}

void ResampleView::setOriginDimension(unsigned int dim1, unsigned int dim2, unsigned int dim3){
    m_Controls.originDim1->setValue(dim1);
    m_Controls.originDim2->setValue(dim2);
    m_Controls.originDim3->setValue(dim3);
}

void ResampleView::setResampleDimension(unsigned int dim1, unsigned int dim2, unsigned int dim3){
    m_Controls.resampleDim1->setValue(dim1);
    m_Controls.resampleDim2->setValue(dim2);
    m_Controls.resampleDim3->setValue(dim3);
}