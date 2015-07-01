/**
* Graphcut pipeline. Inherits from the WorkerBaseClass in order to work correctly with QThreads
*
* TODO:
* (-)
*/

#ifndef __GraphcutWorker_h__
#define __GraphcutWorker_h__

// ITK
#include <itkImage.h>
#include <itkCommand.h>

#include "lib/GraphCut3D/ImageGraphCut3DFilter.h"
#include "Worker.h"

class ProgressObserverCommand : public itk::Command {
public:
    itkNewMacro(ProgressObserverCommand);

    void Execute(itk::Object *caller, const itk::EventObject &event){
        Execute(const_cast<const itk::Object *>(caller), event);
    }

    void Execute(const itk::Object *caller, const itk::EventObject &event){
        itk::ProcessObject *processObject = (itk::ProcessObject*)caller;
        if (typeid(event) == typeid(itk::ProgressEvent)) {
            if(m_worker){
                m_worker->itkProgressCommandCallback(processObject->GetProgress());
            } else{
                std::cout << "ITK Progress event received from "
                        << processObject->GetNameOfClass() << ". Progress is "
                        << 100.0 * processObject->GetProgress() << " %."
                        << std::endl;
            }
        }
    }

    void SetCallbackWorker(Worker* worker){
        m_worker = worker;
    }
private:
    Worker *m_worker;
};

class GraphcutWorker : public Worker {

public:
    enum BoundaryDirection{
        BIDIRECTIONAL = 0,
        BRIGHT_TO_DARK = 1,
        DARK_TO_BRIGHT = 2,
        BoundaryDirection_MAX_VALUE = DARK_TO_BRIGHT
    };

    ~GraphcutWorker(){
    }

    // image typedefs
    typedef itk::Image<short, 3> InputImageType;
    typedef unsigned char BinaryPixelType;
    typedef itk::Image<BinaryPixelType, 3> MaskImageType;
    typedef itk::Image<BinaryPixelType, 3> OutputImageType;

    // typedef for pipeline
    typedef itk::ImageGraphCut3DFilter<InputImageType, MaskImageType, MaskImageType, OutputImageType> GraphCutFilterType;

    GraphcutWorker();

    // inherited slots
    void process();

    // inherited signals
    void started(unsigned int workerId);
    void progress(float progress, unsigned int workerId);
    void finished(itk::DataObject::Pointer ptr, unsigned int workerId);

    // callback for the progress command
    void itkProgressCommandCallback(float progress);

    // setters
    void setInputImage(InputImageType::Pointer img){
        m_input = img;
    }

    void setForegroundMask(MaskImageType::Pointer mask){
        m_foreground = mask;
    }

    void setBackgroundMask(MaskImageType::Pointer mask){
        m_background = mask;
    }

    void setSigma(double d){
        m_Sigma = d;
    }

    void setBoundaryDirection(BoundaryDirection i){
        m_boundaryDirection = i;
    }

    void SetForegroundPixelValue(BinaryPixelType u){
        m_ForegroundPixelValue = u;
    }

    unsigned int id;

private:

    void preparePipeline();

    // member variables
    InputImageType::Pointer m_input;
    MaskImageType::Pointer m_foreground;
    MaskImageType::Pointer m_background;
    OutputImageType::Pointer m_output;
    GraphCutFilterType::Pointer m_graphCut;
    ProgressObserverCommand::Pointer m_progressCommand;

    // parameters
    double m_Sigma;
    BoundaryDirection m_boundaryDirection;
    BinaryPixelType m_ForegroundPixelValue;
};

#endif // __GraphcutWorker_h__

