#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>
#include <QMessageBox>
#include <mitkImage.h>

#include "VolumeMeshView.h"

const std::string VolumeMeshView::VIEW_ID = "org.mitk.views.volumemesher";

void VolumeMeshView::SetFocus(){
  m_Controls.buttonPerformImageProcessing->setFocus();
}

void VolumeMeshView::CreateQtPartControl( QWidget *parent ){
  m_Controls.setupUi( parent );
}