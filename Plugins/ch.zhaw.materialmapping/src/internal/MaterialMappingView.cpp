#include <algorithm>
#include <cmath>

#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>
#include <QMessageBox>
#include <QShortcut>
#include <mitkImage.h>
#include <mitkGridRepresentationProperty.h>
#include <mitkVtkScalarModeProperty.h>
#include <mitkTransferFunction.h>
#include <mitkTransferFunctionProperty.h>
#include <mitkVtkMapper.h>
#include <mitkVtkGLMapperWrapper.h>
#include <mitkUnstructuredGridVtkMapper3D.h>
//#include <mitkUnstructuredGridMapper2D.h>

#include "MaterialMappingView.h"
#include "lib/WorkbenchUtils/WorkbenchUtils.h"
#include "GuiHelpers.h"
#include "MaterialMappingFilter.h"
#include "PowerLawWidget.h"
#include "UnstructuredGridMapper2D.h"

const std::string MaterialMappingView::VIEW_ID = "org.mitk.views.materialmapping";
Ui::MaterialMappingViewControls *MaterialMappingView::controls = nullptr;

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
        controls = &m_Controls;
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
    connect( m_Controls.addPowerLawButton, SIGNAL(clicked()), m_PowerLawWidgetManager.get(), SLOT(addPowerLaw()) );
    connect( m_Controls.removePowerLawButton, SIGNAL(clicked()), m_PowerLawWidgetManager.get(), SLOT(removePowerLaw()) );
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
        filter->SetPowerLawFunctor(m_PowerLawWidgetManager->createFunctor());
        filter->SetDoPeelStep(m_Controls.uParamCheckBox->isChecked());
        filter->SetNumberOfExtendImageSteps(m_Controls.eParamSpinBox->value());
        filter->SetMinElementValue(m_Controls.fParamSpinBox->value());

        auto result = filter->GetOutput();
        filter->Update();

        mitk::DataNode::Pointer newNode = mitk::DataNode::New();
        newNode->SetData(result);

        // set some node properties
        newNode->SetProperty("name", mitk::StringProperty::New("material mapped mesh"));
        newNode->SetProperty("layer", mitk::IntProperty::New(1));

        // http://docs.mitk.org/2015.05/classmitk_1_1GridRepresentationProperty.html
        newNode->SetProperty("grid representation", mitk::GridRepresentationProperty::New(0));
        newNode->SetProperty("scalar mode", mitk::VtkScalarModeProperty::New(3));
        newNode->SetProperty("visible", mitk::BoolProperty::New(true));
        newNode->SetProperty("scalar visibility", mitk::BoolProperty::New(true));
        newNode->SetProperty("outline polygons", mitk::BoolProperty::New(true));

        auto tf = mitk::TransferFunction::New();
        auto min = 0;
        auto max = 100;
        tf->SetMin(min);
        tf->SetMax(max);
        tf->GetColorTransferFunction()->AddRGBPoint ( min, 0.23, 0.29, 0.75 );
        tf->GetColorTransferFunction()->AddRGBPoint ( (min + max) / 2.0, 0.9, 0.9, 0.9 );
        tf->GetColorTransferFunction()->AddRGBPoint ( max, 0.70, 0.0, 0.15 );
        tf->GetScalarOpacityFunction()->AddPoint ( tf->GetColorTransferFunction()->GetRange() [0], 1 );
        tf->GetScalarOpacityFunction()->AddPoint ( tf->GetColorTransferFunction()->GetRange() [1], 1 );
        newNode->SetProperty ( "TransferFunction", mitk::TransferFunctionProperty::New ( tf ) );

        auto renderer = mitk::BaseRenderer::GetInstance(mitk::BaseRenderer::GetRenderWindowByName("stdmulti.widget1"));
//        auto mapper2D = dynamic_cast<mitk::VtkMapper*>(newNode->GetMapper(mitk::BaseRenderer::Standard2D)); //VtkGLMapperWrapper
//        mapper2D->Update(renderer);
        auto mapper2D = mitk::UnstructuredGridPointMapper2D::New();
        newNode->SetMapper(mitk::BaseRenderer::Standard2D, mapper2D);

        auto mapper3D = dynamic_cast<mitk::UnstructuredGridVtkMapper3D*>(newNode->GetMapper(mitk::BaseRenderer::Standard3D));
//        mapper2D->SelectColorArray("E");


//        auto prop2d = mapper2D->GetVtkProp(renderer)->GetWrappedGLMapper();
        auto prop3d = mapper3D->GetVtkProp(renderer);
        auto assembly = dynamic_cast<vtkAssembly*>(mapper3D->GetVtkProp(renderer));
        if(assembly){
            auto collection = assembly->GetParts();
            collection->InitTraversal();
            vtkProp* prop;
            do{
                prop = collection->GetNextProp();
                auto actor = dynamic_cast<vtkActor*>(prop);
                if(actor){
                    actor->GetMapper()->SelectColorArray("E");
                }
            } while(prop != collection->GetLastProp());
        }

//        auto mapper2 = static_cast<mitk::UnstructuredGridMapper2D*>(mapper);


        auto list = newNode->GetPropertyList();
        auto map = list->GetMap();

        for(const auto &item : *map){
            MITK_INFO << item.first;
        }

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
