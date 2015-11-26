#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>
#include <QMessageBox>
#include <QShortcut>
#include <mitkImage.h>

#include "MaterialMappingView.h"
#include "lib/WorkbenchUtils/WorkbenchUtils.h"
#include "GuiHelpers.h"

const std::string MaterialMappingView::VIEW_ID = "org.mitk.views.materialmapping";

void MaterialMappingView::CreateQtPartControl(QWidget *parent) {
    m_Controls.setupUi(parent);
    // table
    auto table = m_Controls.calibrationTableView;
    table->setModel(m_CalibrationDataModel.getQItemModel());
    auto setResizeMode = [=](int _column, QHeaderView::ResizeMode _mode){
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0) // renamed in 5.0
        table->horizontalHeader()->setResizeMode(_column, _mode);
#else
        table->horizontalHeader()->setSectionResizeMode(_column, _mode);
#endif
    };
    setResizeMode(0, QHeaderView::Stretch);
    setResizeMode(1, QHeaderView::Stretch);

    // data selectors
    m_Controls.unstructuredGridComboBox->SetDataStorage(this->GetDataStorage());
    m_Controls.unstructuredGridComboBox->SetAutoSelectNewItems(false);
    m_Controls.unstructuredGridComboBox->SetPredicate(WorkbenchUtils::createIsUnstructuredGridTypePredicate());

    m_Controls.greyscaleImageComboBox->SetDataStorage(this->GetDataStorage());
    m_Controls.greyscaleImageComboBox->SetAutoSelectNewItems(false);
    m_Controls.greyscaleImageComboBox->SetPredicate(WorkbenchUtils::createIsImageTypePredicate());

    // delete key on table
    QShortcut* shortcut = new QShortcut(QKeySequence(QKeySequence::Delete), table);
    connect(shortcut, SIGNAL(activated()), this, SLOT(deleteSelectedRows()));

    // signals
    connect( m_Controls.loadButton, SIGNAL(clicked()), this, SLOT(loadButtonClicked()) );
    connect( m_Controls.saveButton, SIGNAL(clicked()), this, SLOT(saveButtonClicked()) );
    connect( m_Controls.startButton, SIGNAL(clicked()), this, SLOT(startButtonClicked()) );
}

void MaterialMappingView::deleteSelectedRows(){
    auto selection = m_Controls.calibrationTableView->selectionModel();
    auto selectedItems = selection->selectedRows();
    std::set<int> rowsToDelete;

    foreach(auto item, selectedItems){
        rowsToDelete.insert(item.row());
    }

    for(std::set<int>::reverse_iterator rit = rowsToDelete.rbegin(); rit != rowsToDelete.rend(); ++rit){
        m_CalibrationDataModel.removeRow(*rit);
    }
}

void MaterialMappingView::loadButtonClicked() {
    m_CalibrationDataModel.openLoadFileDialog();
}

void MaterialMappingView::saveButtonClicked() {
    m_CalibrationDataModel.openSaveFileDialog();
}

void MaterialMappingView::startButtonClicked() {
    if(isValidSelection()){
        MITK_INFO("ch.zhaw.materialmapping") << "processing input";
        mitk::DataNode *imageNode = m_Controls.greyscaleImageComboBox->GetSelectedNode();
        mitk::DataNode *ugridNode = m_Controls.unstructuredGridComboBox->GetSelectedNode();

        mitk::Image::Pointer image = dynamic_cast<mitk::Image *>(imageNode->GetData());
        mitk::UnstructuredGrid::Pointer ugrid = dynamic_cast<mitk::UnstructuredGrid *>(ugridNode->GetData());
    }
}

bool MaterialMappingView::isValidSelection() {
    // get the nodes selected
    mitk::DataNode *imageNode = m_Controls.greyscaleImageComboBox->GetSelectedNode();
    mitk::DataNode *ugridNode = m_Controls.unstructuredGridComboBox->GetSelectedNode();

    // set the mandatory field based on whether or not the nodes are NULL
    gui::setMandatoryField(m_Controls.greyscaleSelector, (imageNode == nullptr));
    gui::setMandatoryField(m_Controls.meshSelector, (ugridNode == nullptr));

    if(imageNode && ugridNode){
        mitk::Image::Pointer image = dynamic_cast<mitk::Image *>(imageNode->GetData());
        mitk::UnstructuredGrid::Pointer ugrid = dynamic_cast<mitk::UnstructuredGrid *>(ugridNode->GetData());

        if(image && ugrid){
            return true;
        } else{
            QString msg("Invalid data. Select an image and a unstructured grid.");
            QMessageBox::warning ( NULL, "Error", msg);
        }
    }
    return false;
}
