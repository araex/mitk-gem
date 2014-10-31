#include "GraphcutWorker.h"
#include "lib/WorkbenchUtils/WorkbenchUtils.h"

GraphcutWorker::GraphcutWorker()
        : id(WorkbenchUtils::getId())
        , m_Sigma(50)
{
}

void GraphcutWorker::preparePipeline() {
    MITK_DEBUG("ch.zhaw.graphcut") << "prepare pipeline...";

    m_graphCut = GraphCutFilterType::New();
    m_graphCut->SetInputImage(m_input);
    m_graphCut->SetForegroundImage(m_foreground);
    m_graphCut->SetBackgroundImage(m_background);

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

    MITK_DEBUG("ch.zhaw.graphcut") << "... pipeline prepared";
}

void GraphcutWorker::process() {
    MITK_DEBUG("ch.zhaw.graphcut") << "worker started";
    emit Worker::started(id);

    try{
        preparePipeline();
        m_graphCut->Update();
        m_output = m_graphCut->GetOutput();
    } catch (itk::ExceptionObject &e){
        std::cerr << "Exception caught during execution of pipeline 'GraphcutWorker'." << std::endl;
        std::cerr << e << std::endl;
    }

    MITK_DEBUG("ch.zhaw.graphcut") << "worker done";
    emit Worker::finished((itk::DataObject::Pointer) m_output, id);
}