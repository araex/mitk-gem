#include "GraphcutWorker.h"
#include "lib/WorkbenchUtils/WorkbenchUtils.h"

GraphcutWorker::GraphcutWorker()
        : id(WorkbenchUtils::getId())
{
}

void GraphcutWorker::preparePipeline() {
    MITK_DEBUG("ch.zhaw.graphcut") << "prepare pipeline...";

    m_graphCut = new Graph3DType();
    m_graphCut->SetImage(m_input);

    //TODO: params from gui
    m_graphCut->SetNumberOfHistogramBins(20);
    m_graphCut->SetSigma(0.05);

    m_graphCut->SetSources(getNonZeroPixelIndices(m_foreground));
    m_graphCut->SetSinks(getNonZeroPixelIndices(m_background));

    MITK_DEBUG("ch.zhaw.graphcut") << "... pipeline prepared";
}

void GraphcutWorker::process() {
    MITK_DEBUG("ch.zhaw.graphcut") << "worker started";
    emit Worker::started(id);

    try{
        preparePipeline();
        m_graphCut->PerformSegmentation();
        m_output = m_graphCut->GetSegmentMask();
    } catch (itk::ExceptionObject &e){
        std::cerr << "Exception caught during execution of pipeline 'GraphcutWorker'." << std::endl;
        std::cerr << e << std::endl;
    }

    MITK_DEBUG("ch.zhaw.graphcut") << "worker done";
    emit Worker::finished((itk::DataObject::Pointer) m_output, id);
}

std::vector<itk::Index<3> > GraphcutWorker::getNonZeroPixelIndices(MaskImageType::Pointer mask) {
    std::vector<itk::Index<3> > nonZeroPixelIndices;

    itk::ImageRegionConstIterator<MaskImageType> regionIterator(mask, mask->GetLargestPossibleRegion());
    while(!regionIterator.IsAtEnd()) {
        if(regionIterator.Get() > itk::NumericTraits<typename MaskImageType::PixelType>::Zero) {
            nonZeroPixelIndices.push_back(regionIterator.GetIndex());
        }
        ++regionIterator;
    }

    return nonZeroPixelIndices;
}