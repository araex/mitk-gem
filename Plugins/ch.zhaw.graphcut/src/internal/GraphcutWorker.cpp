/**
 *  MITK-GEM: Graphcut Plugin
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#include <thread>
#include <itkBinaryThresholdImageFilter.h>

#include "GraphcutWorker.h"
#include "WorkbenchUtils.h"

GraphcutWorker::GraphcutWorker()
        : id(WorkbenchUtils::getId())
        , m_Sigma(50)
        , m_ForegroundPixelValue(255)
{
}

void GraphcutWorker::preparePipeline() {
    MITK_INFO("ch.zhaw.graphcut") << "prepare pipeline...";

    m_graphCut = GraphCutFilterType::New();
    m_graphCut->SetInputImage(m_input);
    m_graphCut->SetForegroundImage(rescaleMask(m_foreground, m_ForegroundPixelValue));
    m_graphCut->SetBackgroundImage(rescaleMask(m_background, m_ForegroundPixelValue));
    m_graphCut->SetForegroundPixelValue(m_ForegroundPixelValue);
    const uint32_t uiNumberOfThreads = std::thread::hardware_concurrency();
    m_graphCut->SetNumberOfThreads(uiNumberOfThreads > 0 ? uiNumberOfThreads : 1);

    m_graphCut->SetSigma(m_Sigma);
    switch (m_boundaryDirection) {
        case 0:
            m_graphCut->SetBoundaryDirectionTypeToNoDirection();
            break;
        case 1:
            m_graphCut->SetBoundaryDirectionTypeToBrightDark();
            break;
        case 2:
            m_graphCut->SetBoundaryDirectionTypeToDarkBright();
            break;
    }

    // add progress observer
    m_progressCommand = ProgressObserverCommand::New();
    static_cast<ProgressObserverCommand*>(m_progressCommand.GetPointer())->SetCallbackWorker(this);
    m_graphCut->AddObserver(itk::ProgressEvent(), m_progressCommand);

    MITK_INFO("ch.zhaw.graphcut") << "... pipeline prepared";
}

void GraphcutWorker::process() {
    MITK_INFO("ch.zhaw.graphcut") << "worker started";
    emit Worker::started(id);

    try{
        preparePipeline();
        m_graphCut->Update();
        m_output = m_graphCut->GetOutput();
    } catch (itk::ExceptionObject &e){
        MITK_ERROR("ch.zhaw.graphcut") << "Exception caught during execution of pipeline 'GraphcutWorker'.";
        MITK_ERROR("ch.zhaw.graphcut") << e;
    }

    MITK_INFO("ch.zhaw.graphcut") << "worker done";
    emit Worker::finished((itk::DataObject::Pointer) m_output, id);
}

void GraphcutWorker::itkProgressCommandCallback(float progress){
    emit Worker::progress(progress, id);
}

GraphcutWorker::MaskImageType::Pointer GraphcutWorker::rescaleMask(MaskImageType::Pointer _mask, MaskImageType::ValueType _insideValue) {
    auto min = itk::NumericTraits<MaskImageType::ValueType>::min();
    auto max = itk::NumericTraits<MaskImageType::ValueType>::max();

    auto thresholdFilter = itk::BinaryThresholdImageFilter<MaskImageType, MaskImageType>::New();
    thresholdFilter->SetInput(_mask);
    thresholdFilter->SetLowerThreshold(min + 1);
    thresholdFilter->SetUpperThreshold(max);
    thresholdFilter->SetInsideValue(_insideValue);
    thresholdFilter->Update();
    return thresholdFilter->GetOutput();
}