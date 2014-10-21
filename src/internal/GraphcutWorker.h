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

#include "lib/GraphCut3D/ImageGraphCut3D.h"
#include "Worker.h"

class GraphcutWorker : public Worker {

public:
    ~GraphcutWorker(){
        delete m_graphCut;
    }
    // image typedefs
    typedef typename itk::Image<short, 3> InputImageType;
    typedef typename itk::Image<unsigned char, 3> MaskImageType;
    typedef typename itk::Image<unsigned char, 3> OutputImageType;

    // typedef for pipeline
    typedef ImageGraphCut3D<InputImageType> Graph3DType;

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

    unsigned int id;

private:

    void preparePipeline();
    std::vector<itk::Index<3> > getNonZeroPixelIndices(MaskImageType::Pointer);

    // member variables
    InputImageType::Pointer m_input;
    MaskImageType::Pointer m_foreground;
    MaskImageType::Pointer m_background;
    OutputImageType::Pointer m_output;
    Graph3DType *m_graphCut;
    double m_Sigma;
};

#endif // __GraphcutWorker_h__

