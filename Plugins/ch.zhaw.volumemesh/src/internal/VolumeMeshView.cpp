/**
 *  MITK-GEM: Volume Mesher Plugin
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *  Copyright (c) 2016, ETH Zurich, Institute for Biomechanics, B. Helgason
 *  Copyright (c) 2016, University of Iceland, Mechanical Engineering and Computer Science, H. Pállson
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#include "MesherCGAL.h"
#include "MesherTetgen.h"
#include "SurfaceToUnstructuredGridFilter.h"
#include "VolumeMeshView.h"
#include "WorkbenchUtils.h"
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>
#include <mitkGridRepresentationProperty.h>
#include <mitkImage.h>
#include <mitkProgressBar.h>
#include <mitkSurface.h>
#include <vtkUnstructuredGrid.h>
#include <QMessageBox>
#include <QtConcurrentRun>


const std::string VolumeMeshView::VIEW_ID = "org.mitk.views.volumemesher";

VolumeMeshView::~VolumeMeshView() {
    if(m_WorkerFuture.isRunning()){
        QMessageBox::warning(0, "", "Volume mesher is still processing data. Waiting for task to finish...");
        m_WorkerFuture.waitForFinished();
    }
}

void VolumeMeshView::SetFocus() {
    m_Controls.generateButton->setFocus();
}

void VolumeMeshView::CreateQtPartControl(QWidget *parent) {
    m_Controls.setupUi(parent);

    // data selectors
    m_Controls.surfaceComboBox->SetDataStorage(this->GetDataStorage());
    m_Controls.surfaceComboBox->SetAutoSelectNewItems(false);
    m_Controls.surfaceComboBox->SetPredicate(WorkbenchUtils::createIsSurfaceTypePredicate());

    // tetgen options
    tetgenbehavior options;
    options.plc = 1;
    options.quality = 1;
    options.nobisect = 1;
    options.fixedvolume = 1;

    m_TetgenOptionGrid.setDefaultOptions(options);
    m_TetgenOptionGrid.addOption("-p", "Tetrahedralizes a piecewise linear complex (PLC).", &tetgenbehavior::plc);
    m_TetgenOptionGrid.addOption("-q", "Refines mesh (to improve mesh quality).", &tetgenbehavior::quality);
    m_TetgenOptionGrid.addOption("-Y", "Preserves the input surface mesh (does not modify it).", &tetgenbehavior::nobisect);
    m_TetgenOptionGrid.addOption("-a", "Applies a maximum tetrahedron volume constraint. Assumes uniform mesh density on the surface.", &tetgenbehavior::fixedvolume);
    m_Controls.settingsGroup->layout()->addWidget(&m_TetgenOptionGrid);

    // signals
    connect(m_Controls.generateButton, SIGNAL(clicked()), this, SLOT(generateButtonClicked()));
    connect(this, SIGNAL(invalidMeshingResultDetected()), this, SLOT(meshingFailed()));
}

void VolumeMeshView::generateButtonClicked() {
    mitk::DataNode *surfaceNode = m_Controls.surfaceComboBox->GetSelectedNode();

    if (surfaceNode) {
        mitk::Surface::Pointer surface = dynamic_cast<mitk::Surface *>(surfaceNode->GetData());

        std::shared_ptr<gem::IMesher> spMesher;
        if(m_Controls.radioTetgen->isChecked())
        {
            spMesher = std::make_shared<gem::MesherTetgen>(m_TetgenOptionGrid.getOptionsFromGui());
        }
        else
        {
            spMesher = std::make_shared<gem::MesherCGAL>();
        }

        auto work = [spMesher, surface, this](){
            m_Controls.container->setEnabled(false);
            mitk::ProgressBar::GetInstance()->AddStepsToDo(3);
            mitk::ProgressBar::GetInstance()->Progress();

            auto meshFilter = SurfaceToUnstructuredGridFilter::New();
            meshFilter->SetInput(surface, spMesher);
            meshFilter->Update();

            mitk::DataNode::Pointer newNode = mitk::DataNode::New();
            auto mesh = meshFilter->GetOutput();
            if(mesh->GetVtkUnstructuredGrid()->GetNumberOfPoints() == 0){
                emit invalidMeshingResultDetected();
            } else {
                newNode->SetData(mesh);
                newNode->SetProperty("name", mitk::StringProperty::New("tetrahedral mesh"));
                newNode->SetProperty("layer", mitk::IntProperty::New(1));

                // add result to the storage
                this->GetDataStorage()->Add( newNode );
            }

            mitk::ProgressBar::GetInstance()->Progress(2);
            m_Controls.container->setEnabled(true);
        };

        m_WorkerFuture = QtConcurrent::run(static_cast<std::function<void()>>(work));
    }
}

void VolumeMeshView::meshingFailed() {
    QMessageBox::warning(0, "Error", "Volume meshing failed. Make sure the input surface is closed. Check the <a href='http://araex.github.io/mitk-gem-site/#faq'>MITK-GEM FAQ</a> for more information.");
}