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

#include "WorkbenchUtils.h"

const std::string ResampleView::VIEW_ID = "org.mitk.views.resampleview";

void ResampleView::SetFocus() {
}

void ResampleView::CreateQtPartControl(QWidget * parent) {
    // create GUI widgets from the Qt Designer's .ui file
    m_Controls.setupUi(parent);

    // setup buttons
    connect(m_Controls.resampleButton, SIGNAL(clicked()), this, SLOT(resampleButtonPressed()));
    connect(m_Controls.mul2Button, SIGNAL(clicked()), this, SLOT(mul2ButtonPressed()));
    connect(m_Controls.copyButton, SIGNAL(clicked()), this, SLOT(copyButtonPressed()));
    connect(m_Controls.div2Button, SIGNAL(clicked()), this, SLOT(div2ButtonPressed()));
}

void ResampleView::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/, const QList <mitk::DataNode::Pointer> &nodes) {
    if (nodes.count() == 0) {
        m_Controls.selectedImage->setText("no image selected");
        setOriginDimension(0, 0, 0);
        return;
    } else if (nodes.count() == 1) {
        mitk::DataNode::Pointer node = nodes.at(0);
        mitk::StringProperty *nameProperty = static_cast<mitk::StringProperty *>(node->GetProperty("name"));
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
        unsigned int newDimensions[3];
        newDimensions[0] = m_Controls.resampleDim1->value();
        newDimensions[1] = m_Controls.resampleDim2->value();
        newDimensions[2] = m_Controls.resampleDim3->value();
        bool inplaceResample = m_Controls.inplaceCheckBox->isChecked();
        WorkbenchUtils::Interpolator interpolationMethod = (WorkbenchUtils::Interpolator) m_Controls.interpolatorComboBox->currentIndex();

        mitk::Image::Pointer resultImg = WorkbenchUtils::resampleImage(img, newDimensions, interpolationMethod);

        if (inplaceResample){
            nodes.at(0)->SetData(resultImg);
        } else {
            // create the node and store the result
            mitk::DataNode::Pointer newNode = mitk::DataNode::New();
            newNode->SetData(resultImg);

            // set some node properties
            newNode->SetProperty("name", mitk::StringProperty::New("Resample Result"));

            // add result to the storage
            this->GetDataStorage()->Add( newNode );
        }

        // global reinit to get all the mitk plugins to notice the spacing changes
        mitk::RenderingManager::GetInstance()->InitializeViewsByBoundingObjects(this->GetDataStorage());

        // update our own plugin
        updateDimensions(resultImg);
    }
}

void ResampleView::mul2ButtonPressed() {
    unsigned int dim1 = m_Controls.originDim1->value();
    unsigned int dim2 = m_Controls.originDim2->value();
    unsigned int dim3 = m_Controls.originDim3->value();
    setResampleDimension(dim1 * 2, dim2 * 2, dim3 * 2);
}

void ResampleView::copyButtonPressed() {
    unsigned int dim1 = m_Controls.originDim1->value();
    unsigned int dim2 = m_Controls.originDim2->value();
    unsigned int dim3 = m_Controls.originDim3->value();
    setResampleDimension(dim1, dim2, dim3);
}

void ResampleView::div2ButtonPressed() {
    unsigned int dim1 = m_Controls.originDim1->value();
    unsigned int dim2 = m_Controls.originDim2->value();
    unsigned int dim3 = m_Controls.originDim3->value();
    setResampleDimension(dim1 / 2, dim2 / 2, dim3 / 2);
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