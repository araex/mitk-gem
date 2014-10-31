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

#include "lib/GraphCut3D/ImageGraphCut3DFilter.h"
#include "Worker.h"

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
    typedef typename itk::Image<short, 3> InputImageType;
    typedef typename itk::Image<unsigned char, 3> MaskImageType;
    typedef typename itk::Image<unsigned char, 3> OutputImageType;

    // typedef for pipeline
    typedef itk::ImageGraphCut3DFilter<InputImageType, MaskImageType, MaskImageType, OutputImageType> GraphCutFilterType;

    GraphcutWorker();

    // inherited slots
    void process();

    // inherited signals
    void started(unsigned int workerId);
    void finished(itk::DataObject::Pointer ptr, unsigned int workerId);

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



    unsigned int id;

private:

    void preparePipeline();

    // member variables
    InputImageType::Pointer m_input;
    MaskImageType::Pointer m_foreground;
    MaskImageType::Pointer m_background;
    OutputImageType::Pointer m_output;
    GraphCutFilterType::Pointer m_graphCut;

    double m_Sigma;
    BoundaryDirection m_boundaryDirection;
};

#endif // __GraphcutWorker_h__

