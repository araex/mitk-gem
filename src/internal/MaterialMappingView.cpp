#include <algorithm>
#include <cmath>

#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>
#include <QMessageBox>
#include <QShortcut>
#include <mitkImage.h>

#include "MaterialMappingView.h"
#include "lib/WorkbenchUtils/WorkbenchUtils.h"
#include "GuiHelpers.h"
#include "MaterialMappingFilter.h"
#include "PowerLawWidget.h"

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

    // testing
    if(TESTING){
        m_Controls.testingGroup->show();
        m_Controls.expectedResultComboBox->SetDataStorage(this->GetDataStorage());
        m_Controls.expectedResultComboBox->SetAutoSelectNewItems(false);
        m_Controls.expectedResultComboBox->SetPredicate(WorkbenchUtils::createIsUnstructuredGridTypePredicate());

        m_TestRunner = std::unique_ptr<Testing::Runner>(new Testing::Runner());
        connect( m_Controls.runUnitTestsButton, SIGNAL(clicked()), m_TestRunner.get(), SLOT(runUnitTests()) );
//        connect( m_Controls.selectLogFileButton, SIGNAL(clicked()), m_TestRunner.get(), SLOT(openLogFileDialog()) );
    } else {
        m_Controls.testingGroup->hide();
    }

    // delete key on table
    QShortcut* shortcut = new QShortcut(QKeySequence(QKeySequence::Delete), table);
    connect(shortcut, SIGNAL(activated()), this, SLOT(deleteSelectedRows()));

    // power law widgets
    m_PowerLawWidgetManager = std::unique_ptr<PowerLawWidgetManager>(new PowerLawWidgetManager(m_Controls.powerLawWidgets));

    // signals
    connect( m_Controls.loadButton, SIGNAL(clicked()), &m_CalibrationDataModel, SLOT(openLoadFileDialog()) );
    connect( m_Controls.saveButton, SIGNAL(clicked()), &m_CalibrationDataModel, SLOT(openSaveFileDialog()) );
    connect( m_Controls.startButton, SIGNAL(clicked()), this, SLOT(startButtonClicked()) );
    connect( &m_CalibrationDataModel, SIGNAL(dataChanged()), this, SLOT(tableDataChanged()) );
    connect( m_Controls.addPowerLawButton, SIGNAL(clicked()), this, SLOT(addPowerLawButtonClicked()) );
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
    tableDataChanged();
}

void MaterialMappingView::startButtonClicked() {
    MITK_INFO("ch.zhaw.materialmapping") << "processing input";
    if(isValidSelection()){
        mitk::DataNode *imageNode = m_Controls.greyscaleImageComboBox->GetSelectedNode();
        mitk::DataNode *ugridNode = m_Controls.unstructuredGridComboBox->GetSelectedNode();

        mitk::Image::Pointer image = dynamic_cast<mitk::Image *>(imageNode->GetData());
        mitk::UnstructuredGrid::Pointer ugrid = dynamic_cast<mitk::UnstructuredGrid *>(ugridNode->GetData());

        auto filter = MaterialMappingFilter::New();
        filter->SetInput(ugrid);
        filter->SetIntensityImage(image);
        filter->SetDensityFunctor(gui::createDensityFunctorFromGui(m_Controls, m_CalibrationDataModel));

        auto result = filter->GetOutput();
        filter->Update();

        mitk::DataNode::Pointer newNode = mitk::DataNode::New();
        newNode->SetData(result);

        // set some node properties
        newNode->SetProperty("name", mitk::StringProperty::New("material mapped mesh"));
        newNode->SetProperty("layer", mitk::IntProperty::New(1));

        // add result to the storage
        this->GetDataStorage()->Add( newNode );

        if(TESTING){
            if(m_Controls.testingDoComparisonCheckBox->isChecked()){
                mitk::DataNode *expectedResultNode = m_Controls.expectedResultComboBox->GetSelectedNode();
                mitk::UnstructuredGrid::Pointer expectedResult = dynamic_cast<mitk::UnstructuredGrid *>(expectedResultNode->GetData());
                m_TestRunner->compareGrids(result, expectedResult);
            }
        }
    }
}

void MaterialMappingView::tableDataChanged() {
    auto linearEqParams = m_CalibrationDataModel.getFittedLine();
    m_Controls.linEQSlopeSpinBox->setValue(linearEqParams.slope);
    m_Controls.linEQOffsetSpinBox->setValue(linearEqParams.offset);
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
    MITK_INFO("ch.zhaw.materialmapping") << "invalid data selection";
    return false;
}

void MaterialMappingView::addPowerLawButtonClicked() {
    m_PowerLawWidgetManager->addPowerLaw();
    MITK_INFO("ch.zhaw.materialmapping") << m_PowerLawWidgetManager->createFunctor();
}
