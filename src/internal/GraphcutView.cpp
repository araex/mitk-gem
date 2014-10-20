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

// Qt
#include <QMessageBox>

// Utils
#include "lib/WorkbenchUtils/WorkbenchUtils.h"

// Graphcut
#include "lib/GraphCut3D/ImageGraphCut3D.h"


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
}

void GraphcutView::OnSelectionChanged(berry::IWorkbenchPart::Pointer, const QList <mitk::DataNode::Pointer> &) {
    MITK_DEBUG("ch.zhaw.graphcut") << "selection changed";
}

void GraphcutView::startButtonPressed() {
    MITK_DEBUG("ch.zhaw.graphcut") << "start button pressed";

    mitk::DataNode *greyscaleImageNode = m_Controls.greyscaleImageSelector->GetSelectedNode();
    mitk::DataNode *foregroundMaskNode = m_Controls.foregroundImageSelector->GetSelectedNode();
    mitk::DataNode *backgroundMaskNode = m_Controls.backgroundImageSelector->GetSelectedNode();

    if(isValidSelection()){
        MITK_INFO("ch.zhaw.graphcut") << "processing input";

        // gather input images
        mitk::Image::Pointer greyscaleImage = dynamic_cast<mitk::Image *>(greyscaleImageNode->GetData());
        mitk::Image::Pointer foregroundMask = dynamic_cast<mitk::Image *>(foregroundMaskNode->GetData());
        mitk::Image::Pointer backgroundMask = dynamic_cast<mitk::Image *>(backgroundMaskNode->GetData());

        // get pixel indices
        std::vector<itk::Index<3> > foregroundPixels = getNonZeroPixelIndices(foregroundMask);
        MITK_INFO("ch.zhaw.graphcut") << "found " <<foregroundPixels.size()<<" pixels";
        std::vector<itk::Index<3> > backgroundPixels = getNonZeroPixelIndices(backgroundMask);
        MITK_INFO("ch.zhaw.graphcut") << "found " <<backgroundPixels.size()<<" pixels";

        // perform graph cut
        MITK_INFO("ch.zhaw.graphcut") << "performing graph cut";

        typedef itk::Image<unsigned short, 3> TImage;
        typedef ImageGraphCut3D<TImage> Graph3DType;
        Graph3DType GraphCut;
        TImage::Pointer greyscaleImageItk;
        mitk::CastToItkImage(greyscaleImage, greyscaleImageItk);
        GraphCut.SetImage(greyscaleImageItk);
        GraphCut.SetNumberOfHistogramBins(20);
        GraphCut.SetSigma(0.05);
        GraphCut.SetSources(foregroundPixels);
        GraphCut.SetSinks(backgroundPixels);
        GraphCut.PerformSegmentation();

        // Get the output
        mitk::Image::Pointer resultImage;
        Graph3DType::ResultImageType::Pointer resultImageItk = GraphCut.GetSegmentMask();
        mitk::CastToMitkImage(resultImageItk,resultImage);
        mitk::DataNode::Pointer newNode = mitk::DataNode::New();
        newNode->SetData( resultImage );

        // set some properties
        newNode->SetProperty("binary", mitk::BoolProperty::New(true));
        newNode->SetProperty("name", mitk::StringProperty::New("graphcut segmentation"));
        newNode->SetProperty("color", mitk::ColorProperty::New(1.0,0.0,0.0));
        newNode->SetProperty("volumerendering", mitk::BoolProperty::New(true));
        newNode->SetProperty("layer", mitk::IntProperty::New(1));
        newNode->SetProperty("opacity", mitk::FloatProperty::New(0.5));

        // add result to data tree
        this->GetDataStorage()->Add( newNode );
        mitk::RenderingManager::GetInstance()->RequestUpdateAll();
    }
}

std::vector<itk::Index<3> > GraphcutView::getNonZeroPixelIndices(mitk::Image::Pointer mitkImage) {
    // cast mitk to itk image (copies memory, will get freed once itkImg gets out of scope)
    typedef itk::Image<unsigned short, 3> TImage;
    TImage::Pointer itkImage = TImage::New();
    mitk::CastToItkImage(mitkImage, itkImage);

    std::vector<itk::Index<3> > nonZeroPixelIndices;

    itk::ImageRegionConstIterator<TImage> regionIterator(itkImage, itkImage->GetLargestPossibleRegion());
    while(!regionIterator.IsAtEnd()) {
        if(regionIterator.Get() > itk::NumericTraits<typename TImage::PixelType>::Zero) {
            nonZeroPixelIndices.push_back(regionIterator.GetIndex());
        }
        ++regionIterator;
    }

    return nonZeroPixelIndices;
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