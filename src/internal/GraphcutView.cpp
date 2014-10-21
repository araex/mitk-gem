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
#include <QmitkDataStorageComboBox.h>
#include "GraphcutView.h"

// MITK
#include <mitkNodePredicateDataType.h>
#include <mitkNodePredicateOr.h>
#include <mitkImageCast.h>
#include <mitkITKImageImport.h>

// Qt
#include <QThreadPool>
#include <QMessageBox>

// Utils
#include "lib/WorkbenchUtils/WorkbenchUtils.h"

// Graphcut
#include "lib/GraphCut3D/ImageGraphCut3D.h"
#include "GraphcutWorker.h"


const std::string GraphcutView::VIEW_ID = "org.mitk.views.imagegraphcut3dsegmentation";

void GraphcutView::SetFocus() {
}

void GraphcutView::CreateQtPartControl(QWidget *parent) {
    // create GUI widgets from the Qt Designer's .ui file
    m_Controls.setupUi(parent);

    // init image selectors
    initializeImageSelector(m_Controls.greyscaleImageSelector);
    initializeImageSelector(m_Controls.foregroundImageSelector);
    initializeImageSelector(m_Controls.backgroundImageSelector);

    // set predicates to filter which images are selectable
    m_Controls.greyscaleImageSelector->SetPredicate(WorkbenchUtils::createIsImageTypePredicate());
    m_Controls.foregroundImageSelector->SetPredicate(WorkbenchUtils::createIsBinaryImageTypePredicate());
    m_Controls.backgroundImageSelector->SetPredicate(WorkbenchUtils::createIsBinaryImageTypePredicate());

    // setup signals
    connect(m_Controls.startButton, SIGNAL(clicked()), this, SLOT(startButtonPressed()));
    connect(m_Controls.greyscaleImageSelector, SIGNAL(OnSelectionChanged (const mitk::DataNode *)), this, SLOT(imageSelectionChanged()));
    connect(m_Controls.foregroundImageSelector, SIGNAL(OnSelectionChanged (const mitk::DataNode *)), this, SLOT(imageSelectionChanged()));
    connect(m_Controls.backgroundImageSelector, SIGNAL(OnSelectionChanged (const mitk::DataNode *)), this, SLOT(imageSelectionChanged()));

    // init defaults
    m_currentlyActiveWorkerCount = 0;
    lockGui(false);
}

void GraphcutView::OnSelectionChanged(berry::IWorkbenchPart::Pointer, const QList <mitk::DataNode::Pointer> &) {
    MITK_DEBUG("ch.zhaw.graphcut") << "selection changed";
}

void GraphcutView::startButtonPressed() {
    MITK_DEBUG("ch.zhaw.graphcut") << "start button pressed";


    if (isValidSelection()) {
        MITK_INFO("ch.zhaw.graphcut") << "processing input";

        // get the nodes
        mitk::DataNode *greyscaleImageNode = m_Controls.greyscaleImageSelector->GetSelectedNode();
        mitk::DataNode *foregroundMaskNode = m_Controls.foregroundImageSelector->GetSelectedNode();
        mitk::DataNode *backgroundMaskNode = m_Controls.backgroundImageSelector->GetSelectedNode();

        // gather input images
        mitk::Image::Pointer greyscaleImage = dynamic_cast<mitk::Image *>(greyscaleImageNode->GetData());
        mitk::Image::Pointer foregroundMask = dynamic_cast<mitk::Image *>(foregroundMaskNode->GetData());
        mitk::Image::Pointer backgroundMask = dynamic_cast<mitk::Image *>(backgroundMaskNode->GetData());

        // create worker. QThreadPool will take care of the deconstruction of the worker once it has finished
        GraphcutWorker *worker = new GraphcutWorker();

        // cast the images to ITK
        typename GraphcutWorker::InputImageType::Pointer greyscaleImageItk;
        typename GraphcutWorker::MaskImageType::Pointer foregroundMaskItk;
        typename GraphcutWorker::MaskImageType::Pointer backgroundMaskItk;
        mitk::CastToItkImage(greyscaleImage, greyscaleImageItk);
        mitk::CastToItkImage(foregroundMask, foregroundMaskItk);
        mitk::CastToItkImage(backgroundMask, backgroundMaskItk);

        // set images in worker
        worker->setInputImage(greyscaleImageItk);
        worker->setForegroundMask(foregroundMaskItk);
        worker->setBackgroundMask(backgroundMaskItk);

        // set parameters
        worker->setSigma(m_Controls.paramSigmaSpinBox->value());
        worker->setBoundaryDirection((GraphcutWorker::BoundaryDirection) m_Controls.paramBoundaryDirectionComboBox->currentIndex());

        // set up signals
        qRegisterMetaType<itk::DataObject::Pointer>("itk::DataObject::Pointer");
        QObject::connect(worker, SIGNAL(started(unsigned int)), this, SLOT(workerHasStarted(unsigned int)));
        QObject::connect(worker, SIGNAL(finished(itk::DataObject::Pointer, unsigned int)), this, SLOT(workerIsDone(itk::DataObject::Pointer, unsigned int)));

        QThreadPool::globalInstance()->start(worker);
    }
}

void GraphcutView::workerHasStarted(unsigned int workerId) {
    MITK_DEBUG("ch.zhaw.graphcut") << "worker " << workerId << " started";
    m_currentlyActiveWorkerCount++;
    lockGui(true);
}

void GraphcutView::workerIsDone(itk::DataObject::Pointer data, unsigned int workerId){
    MITK_DEBUG("ch.zhaw.graphcut") << "worker " << workerId << " finished";

    // cast the image back to mitk
    GraphcutWorker::OutputImageType *resultImageItk = dynamic_cast<GraphcutWorker::OutputImageType *>(data.GetPointer());
    mitk::Image::Pointer resultImage = mitk::GrabItkImageMemory(resultImageItk);

    // create the node
    mitk::DataNode::Pointer newNode = mitk::DataNode::New();
    newNode->SetData(resultImage);

    // set some properties
    newNode->SetProperty("binary", mitk::BoolProperty::New(true));
    newNode->SetProperty("name", mitk::StringProperty::New("graphcut segmentation"));
    newNode->SetProperty("color", mitk::ColorProperty::New(1.0,0.0,0.0));
    newNode->SetProperty("volumerendering", mitk::BoolProperty::New(true));
    newNode->SetProperty("layer", mitk::IntProperty::New(1));
    newNode->SetProperty("opacity", mitk::FloatProperty::New(0.5));

    // add result to data tree
    this->GetDataStorage()->Add( newNode );

    // update gui
    if(--m_currentlyActiveWorkerCount == 0){ // no more active workers
        lockGui(false);
    }
    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void GraphcutView::imageSelectionChanged() {
    MITK_DEBUG("ch.zhaw.graphcut") << "selector changed image";
}

void GraphcutView::initializeImageSelector(QmitkDataStorageComboBox *selector){
    selector->SetDataStorage(this->GetDataStorage());
    selector->SetAutoSelectNewItems(false);
}

void GraphcutView::setMandatoryField(QWidget *widget, bool bMandatory){
    widget->setProperty("mandatoryField", bMandatory);
    widget->style()->unpolish(widget); // need to do this since we changed the stylesheet
    widget->style()->polish(widget);
    widget->update();
}

bool GraphcutView::isValidSelection() {
    // get the nodes selected
    mitk::DataNode *greyscaleImageNode = m_Controls.greyscaleImageSelector->GetSelectedNode();
    mitk::DataNode *foregroundMaskNode = m_Controls.foregroundImageSelector->GetSelectedNode();
    mitk::DataNode *backgroundMaskNode = m_Controls.backgroundImageSelector->GetSelectedNode();

    // set the mandatory field based on whether or not the nodes are NULL
    setMandatoryField(m_Controls.greyscaleSelector, (greyscaleImageNode==NULL));
    setMandatoryField(m_Controls.foregroundSelector, (foregroundMaskNode==NULL));
    setMandatoryField(m_Controls.backgroundSelector, (backgroundMaskNode==NULL));

    if(greyscaleImageNode && foregroundMaskNode && backgroundMaskNode){
        MITK_DEBUG("ch.zhaw.graphcut") << "valid selection";
        return true;
    } else{
        MITK_DEBUG("ch.zhaw.graphcut") << "invalid selection";
        return false;
    }
}

void GraphcutView::lockGui(bool b) {
    m_Controls.parentWidget->setEnabled(!b);
    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}