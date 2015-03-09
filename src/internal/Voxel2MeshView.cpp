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

    // connect slots
    connect(m_Controls.generateSurfaceButton, SIGNAL(clicked()), this, SLOT(generateSurfaceButtonPressed()));
}

void Voxel2MeshView::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/, const QList <mitk::DataNode::Pointer> &nodes) {
    if(nodes.count() == 0) {
        m_Controls.selectedImages->setText("no segmentation selected");
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

void Voxel2MeshView::generateSurfaceButtonPressed() {
    MITK_INFO("ch.zhaw.voxel2mesh") << "Generating surfaces";

    // get data
    QList <mitk::DataNode::Pointer> nodes = this->GetDataManagerSelection();
    SurfaceGeneratorParameters params = getParameters();

    foreach(mitk::DataNode::Pointer node, nodes){
        mitk::Image::Pointer img = dynamic_cast<mitk::Image *>(node->GetData());
        mitk::Surface::Pointer surface = createSurface(img);
        mitk::DataNode::Pointer surfaceNode = mitk::DataNode::New();
        QString name("surface");
        surfaceNode->SetProperty("name", mitk::StringProperty::New(name.toUtf8().constData()));
        surfaceNode->SetData(surface);
        this->GetDataStorage()->Add( surfaceNode );
    }

    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

mitk::Surface::Pointer Voxel2MeshView::createSurface(mitk::Image::Pointer img){
    // TODO: do the actual work
}

Voxel2MeshView::SurfaceGeneratorParameters Voxel2MeshView::getParameters() {
    SurfaceGeneratorParameters ret;
    ret.doMedian = m_Controls.medianGroup->isChecked();
    ret.kernelX = m_Controls.kernelXSpinBox->value();
    ret.kernelY = m_Controls.kernelYSpinBox->value();
    ret.kernelZ = m_Controls.kernelZSpinBox->value();

    ret.doGaussian = m_Controls.gaussGroup->isChecked();
    ret.deviation = m_Controls.deviationSpinBox->value();
    ret.radius = m_Controls.radiusSpinBox->value();

    ret.threshold = m_Controls.thresholdSpinBox->value();

    ret.doSmoothing = m_Controls.smoothingGroup->isChecked();
    ret.iterations = m_Controls.iterationSpinBox->value();
    ret.relaxation = m_Controls.relaxationSpinBox->value();

    ret.doDecimation = m_Controls.decimationGroup->isChecked();
    ret.reduction = m_Controls.reductionSpinBox->value();

    return ret;
}