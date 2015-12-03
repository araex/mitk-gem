#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>
#include <QMessageBox>
#include <mitkImage.h>

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

    // signals
    connect(m_Controls.generateButton, SIGNAL(clicked()), this, SLOT(generateButtonClicked()));
}

void VolumeMeshView::generateButtonClicked() {
    mitk::DataNode *surfaceNode = m_Controls.surfaceComboBox->GetSelectedNode();

    if (surfaceNode) {
        mitk::Surface::Pointer surface = dynamic_cast<mitk::Surface *>(surfaceNode->GetData());

        auto meshFilter = SurfaceToUnstructuredGridFilter::New();
        meshFilter->SetInput(surface);
        meshFilter->Update();

        mitk::DataNode::Pointer newNode = mitk::DataNode::New();
        newNode->SetData(meshFilter->GetOutput());
        newNode->SetProperty("name", mitk::StringProperty::New("tetrahedral mesh"));
        newNode->SetProperty("layer", mitk::IntProperty::New(1));

        // add result to the storage
        this->GetDataStorage()->Add( newNode );
    }

}