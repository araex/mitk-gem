#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>
#include <QMessageBox>
#include <mitkImage.h>
#include <mitkGridRepresentationProperty.h>


#include "VolumeMeshView.h"
#include "SurfaceToUnstructuredGridFilter.h"
#include "lib/WorkbenchUtils/WorkbenchUtils.h"


const std::string VolumeMeshView::VIEW_ID = "org.mitk.views.volumemesher";

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
}

void VolumeMeshView::generateButtonClicked() {
    mitk::DataNode *surfaceNode = m_Controls.surfaceComboBox->GetSelectedNode();

    if (surfaceNode) {
        mitk::Surface::Pointer surface = dynamic_cast<mitk::Surface *>(surfaceNode->GetData());

        auto meshFilter = SurfaceToUnstructuredGridFilter::New();
        meshFilter->SetInput(surface);
        meshFilter->SetTetgenOptions(m_TetgenOptionGrid.getOptionsFromGui());
        meshFilter->Update();

        mitk::DataNode::Pointer newNode = mitk::DataNode::New();
        newNode->SetData(meshFilter->GetOutput());
        newNode->SetProperty("name", mitk::StringProperty::New("tetrahedral mesh"));
        newNode->SetProperty("layer", mitk::IntProperty::New(1));

        // add result to the storage
        this->GetDataStorage()->Add( newNode );
    }

}