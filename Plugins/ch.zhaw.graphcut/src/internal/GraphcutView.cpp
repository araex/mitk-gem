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
#include <mitkNodePredicateNot.h>
#include <mitkTimeGeometry.h>

// Qt
#include <QThreadPool>
#include <QMessageBox>

// Graphcut
#include "lib/GraphCut3D/ImageGraphCut3DFilter.h"
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
    connect(m_Controls.refreshTimeButton, SIGNAL(clicked()), this, SLOT(refreshButtonPressed()));
    connect(m_Controls.refreshMemoryButton, SIGNAL(clicked()), this, SLOT(refreshButtonPressed()));
    connect(m_Controls.greyscaleImageSelector, SIGNAL(OnSelectionChanged (const mitk::DataNode *)), this, SLOT(imageSelectionChanged()));
    connect(m_Controls.foregroundImageSelector, SIGNAL(OnSelectionChanged (const mitk::DataNode *)), this, SLOT(imageSelectionChanged()));
    connect(m_Controls.backgroundImageSelector, SIGNAL(OnSelectionChanged (const mitk::DataNode *)), this, SLOT(imageSelectionChanged()));

    // init default state
    m_currentlyActiveWorkerCount = 0;
    lockGui(false);
}

void GraphcutView::OnSelectionChanged(berry::IWorkbenchPart::Pointer, const QList <mitk::DataNode::Pointer> &) {
    MITK_DEBUG("ch.zhaw.graphcut") << "selection changed";
}

void GraphcutView::startButtonPressed() {
    MITK_INFO("ch.zhaw.graphcut") << "start button pressed";

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
        MITK_INFO("ch.zhaw.graphcut") << "create the worker";
        GraphcutWorker *worker = new GraphcutWorker();

        // cast the images to ITK
        MITK_INFO("ch.zhaw.graphcut") << "cast the images to ITK";
        GraphcutWorker::InputImageType::Pointer greyscaleImageItk;
        GraphcutWorker::MaskImageType::Pointer foregroundMaskItk;
        GraphcutWorker::MaskImageType::Pointer backgroundMaskItk;
        mitk::CastToItkImage(greyscaleImage, greyscaleImageItk);
        mitk::CastToItkImage(foregroundMask, foregroundMaskItk);
        mitk::CastToItkImage(backgroundMask, backgroundMaskItk);

        // set images in worker
        MITK_INFO("ch.zhaw.graphcut") << "init worker";
        worker->setInputImage(greyscaleImageItk);
        worker->setForegroundMask(foregroundMaskItk);
        worker->setBackgroundMask(backgroundMaskItk);

        // set parameters
        worker->setSigma(m_Controls.paramSigmaSpinBox->value());
        worker->setBoundaryDirection((GraphcutWorker::BoundaryDirection) m_Controls.paramBoundaryDirectionComboBox->currentIndex());
        worker->setForegroundPixelValue(m_Controls.paramLabelValueSpinBox->value());

        // set up signals
        MITK_INFO("ch.zhaw.graphcut") << "register signals";
        qRegisterMetaType<itk::DataObject::Pointer>("itk::DataObject::Pointer");
        QObject::connect(worker, SIGNAL(started(unsigned int)), this, SLOT(workerHasStarted(unsigned int)));
        QObject::connect(worker, SIGNAL(finished(itk::DataObject::Pointer, unsigned int)), this, SLOT(workerIsDone(itk::DataObject::Pointer, unsigned int)));
        QObject::connect(worker, SIGNAL(progress(float, unsigned int)), this, SLOT(workerProgressUpdate(float, unsigned int)));

        // prepare the progress bar
        MITK_INFO("ch.zhaw.graphcut") << "prepare GUI";
        m_Controls.progressBar->setValue(0);
        m_Controls.progressBar->setMinimum(0);
        m_Controls.progressBar->setMaximum(100);

        MITK_INFO("ch.zhaw.graphcut") << "start the worker";
        QThreadPool::globalInstance()->start(worker, QThread::HighestPriority);
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

    // create the node and store the result
    mitk::DataNode::Pointer newNode = mitk::DataNode::New();
    newNode->SetData(resultImage);

    // set some node properties
    newNode->SetProperty("binary", mitk::BoolProperty::New(true));
    newNode->SetProperty("name", mitk::StringProperty::New("graphcut segmentation"));
    newNode->SetProperty("color", mitk::ColorProperty::New(1.0,0.0,0.0));
    newNode->SetProperty("volumerendering", mitk::BoolProperty::New(true));
    newNode->SetProperty("layer", mitk::IntProperty::New(1));
    newNode->SetProperty("opacity", mitk::FloatProperty::New(0.5));

    // add result to the storage
    this->GetDataStorage()->Add( newNode );

    // update gui
    if(--m_currentlyActiveWorkerCount == 0){ // no more active workers
        lockGui(false);
    }
    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void GraphcutView::imageSelectionChanged() {
    MITK_DEBUG("ch.zhaw.graphcut") << "selector changed image";

    // estimate required memory and computation time
    mitk::DataNode *greyscaleImageNode = m_Controls.greyscaleImageSelector->GetSelectedNode();
    if(greyscaleImageNode){
        // numberOfVertices is straightforward
        mitk::Image::Pointer greyscaleImage = dynamic_cast<mitk::Image *>(greyscaleImageNode->GetData());
        auto x = greyscaleImage->GetDimension(0);
        auto y = greyscaleImage->GetDimension(1);
        auto z = greyscaleImage->GetDimension(2);
        auto numberOfVertices = x*y*z;

        // numberOfEdges are a bit more tricky
        auto numberOfEdges = 3; // 3 because we're using a 6-connected neighborhood which gives us 3 edges / pixel
        numberOfEdges = (numberOfEdges * x) - 1;
        numberOfEdges = (numberOfEdges * y) - x;
        numberOfEdges = (numberOfEdges * z) - x * y;
        numberOfEdges *= 2; // because kolmogorov adds 2 directed edges instead of 1 bidirectional

        // the input image will be cast to short
        auto itkImageSizeInMemory = numberOfVertices * sizeof(short);

        // both mask are cast to unsigned chars
        itkImageSizeInMemory += (2 * numberOfVertices * sizeof(unsigned char));

        // node struct is 48byte, arc is 28byte as defined by Kolmogorov max flow v3.0.03
        auto memoryRequiredInBytes = numberOfVertices * 48 + numberOfEdges * 28 + itkImageSizeInMemory;

        MITK_INFO("ch.zhaw.graphcut") << "Image has " << numberOfVertices << " vertices and " <<  numberOfEdges << " edges";

        updateMemoryRequirements(memoryRequiredInBytes);
        updateTimeEstimate(numberOfEdges);
    }
}

void GraphcutView::updateMemoryRequirements(double memoryRequiredInBytes){
    QString memory = QString::number(memoryRequiredInBytes / 1024.0 / 1024.0, 'f', 0);
    memory.append("MB");
    m_Controls.estimatedMemory->setText(memory);
    if(memoryRequiredInBytes > 4096000000){
        setErrorField(m_Controls.estimatedMemory, true);
    } else if(memoryRequiredInBytes > 2048000000){
        setErrorField(m_Controls.estimatedMemory, false);
        setWarningField(m_Controls.estimatedMemory, true);
    } else{
        setErrorField(m_Controls.estimatedMemory, false);
        setWarningField(m_Controls.estimatedMemory, false);
    }
    MITK_INFO("ch.zhaw.graphcut") <<  "Representing the full graph will require " << memoryRequiredInBytes << " Bytes of memory to compute.";
}

void GraphcutView::updateTimeEstimate(long long numberOfEdges){
    // trendlines based on dataset of 50 images with incremental sizes calculated on a 32GB machine

    // graph init / reading results. linear
    // y = c0*x + c1
    double c0 = 2.0e-07;
    double c1 = 0.1148;
    double x = numberOfEdges;
    double estimatedSetupAndBreakdownTimeInSeconds = c0*x + c1;

    // the max flow computation.
    // c0*x^(c1)
    c0 = 2.0e-18;
    c1 = 2.4;
    x = numberOfEdges;

    double estimatedComputeTimeInSeconds = c0*pow(x, c1);
    double estimateInSeconds;

    // max flow on < 30mega edges has a irregular time complexity and is thus excluded from the trendline
    if(numberOfEdges < 30000000){
        // max flow is very (<0.03s) fast in this range
        estimateInSeconds = estimatedSetupAndBreakdownTimeInSeconds;
    } else{
        estimateInSeconds = estimatedSetupAndBreakdownTimeInSeconds + estimatedComputeTimeInSeconds * 2; // * 2 because the estimation is off anyways. better estimate pessimistically
    }

    QString time = QString::number(estimateInSeconds, 'f', 2);
    time.append("s");
    m_Controls.estimatedTime->setText(time);
    if(estimateInSeconds > 60){
        setErrorField(m_Controls.estimatedTime, true);
    } else if (estimateInSeconds > 30) {
        setErrorField(m_Controls.estimatedTime, false);
        setWarningField(m_Controls.estimatedTime, true);
    }else {
        setErrorField(m_Controls.estimatedTime, false);
        setWarningField(m_Controls.estimatedTime, false);
    }

    MITK_INFO("ch.zhaw.graphcut") << "Graphcut computation will take about " << estimateInSeconds << " seconds.";
}

void GraphcutView::initializeImageSelector(QmitkDataStorageComboBox *selector){
    selector->SetDataStorage(this->GetDataStorage());
    selector->SetAutoSelectNewItems(false);
}

void GraphcutView::setMandatoryField(QWidget *widget, bool bEnabled){
    setQStyleSheetField(widget, "mandatoryField", bEnabled);
}

void GraphcutView::setWarningField(QWidget *widget, bool bEnabled){
    setQStyleSheetField(widget, "warningField", bEnabled);
}

void GraphcutView::setErrorField(QWidget *widget, bool bEnabled){
    setQStyleSheetField(widget, "errorField", bEnabled);
}

void GraphcutView::setQStyleSheetField(QWidget *widget, const char *fieldName, bool bEnabled){
    widget->setProperty(fieldName, bEnabled);
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
        if(foregroundMaskNode->GetName() == backgroundMaskNode->GetName()){
            setMandatoryField(m_Controls.foregroundSelector, true);
            setMandatoryField(m_Controls.backgroundSelector, true);
            QMessageBox::warning ( NULL, "Error", "foreground and background seem to be the same image.");
            return false;
        }

        // gather input images
        mitk::Image::Pointer grey = dynamic_cast<mitk::Image *>(greyscaleImageNode->GetData());
        mitk::Image::Pointer fg = dynamic_cast<mitk::Image *>(foregroundMaskNode->GetData());
        mitk::Image::Pointer bg = dynamic_cast<mitk::Image *>(backgroundMaskNode->GetData());

        MITK_INFO << grey->GetDimension() << fg->GetDimension() << bg->GetDimension();
        if((grey->GetDimension() == fg->GetDimension()) && (fg->GetDimension() == bg->GetDimension())){
            for(int i = 0, max = grey->GetDimension(); i < max ; ++i){
                if((grey->GetDimensions()[i] == fg->GetDimensions()[i]) && (fg->GetDimensions()[i] == bg->GetDimensions()[i])){
                    continue;
                } else{
                    QString msg("Image dimension mismatch in dimension ");
                    msg.append(QString::number(i));
                    msg.append(". Please resample the images.");
                    QMessageBox::warning ( NULL, "Error", msg);
                    return false;
                }
            }
        } else{
            QMessageBox::warning ( NULL, "Error", "Image dimensions do not match.");
            return false;
        }

        MITK_DEBUG("ch.zhaw.graphcut") << "valid selection";
        return true;
    } else{
        MITK_ERROR("ch.zhaw.graphcut") << "invalid selection: missing input.";
        return false;
    }
}

void GraphcutView::lockGui(bool b) {
    m_Controls.parentWidget->setEnabled(!b);
    m_Controls.progressBar->setVisible(b);
    m_Controls.startButton->setVisible(!b);
    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void GraphcutView::workerProgressUpdate(float progress, unsigned int){
    int progressInt = (int) (progress * 100.0f);
    m_Controls.progressBar->setValue(progressInt);
    mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void GraphcutView::refreshButtonPressed(){
    imageSelectionChanged();
}