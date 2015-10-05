#ifndef _MITKGRAPHCUTSEGMENTATIONTOSURFACEFILTER_h__
#define _MITKGRAPHCUTSEGMENTATIONTOSURFACEFILTER_h__

#include <mitkImageToSurfaceFilter.h>

#include <vtkImageGaussianSmooth.h>
#include <vtkImageMedian3D.h>
#include <vtkImageThreshold.h>


namespace mitk {
    class GraphcutSegmentationToSurfaceFilter : public ImageToSurfaceFilter {
    public:
        mitkClassMacro(GraphcutSegmentationToSurfaceFilter,ImageToSurfaceFilter);

        typedef double vtkDouble;
        virtual void GenerateData() override;
        itkFactorylessNewMacro(Self)
        itkCloneMacro(Self)


        // setter / getter macros
        itkSetMacro(UseMedian,bool);
        itkGetConstMacro(UseMedian,bool);
        itkSetMacro(UseThresholding,bool);
        itkGetConstMacro(UseThresholding,bool);
        void SetMedianKernelSize(int x, int y, int z) {
            m_MedianKernelSizeX = x;
            m_MedianKernelSizeY = y;
            m_MedianKernelSizeZ = z;
        }
        itkSetMacro(MedianKernelSizeX, int);
        itkGetConstMacro(MedianKernelSizeX, int);
        itkSetMacro(MedianKernelSizeY, int);
        itkGetConstMacro(MedianKernelSizeY, int);
        itkSetMacro(MedianKernelSizeZ, int);
        itkGetConstMacro(MedianKernelSizeZ, int);

        itkSetMacro(UseGaussianSmoothing,bool);
        itkGetConstMacro(UseGaussianSmoothing,bool);
        itkSetMacro(GaussianStandardDeviation, double);
        itkGetConstMacro(GaussianStandardDeviation, double);
        itkSetMacro(GaussianRadius, double);
        itkGetConstMacro(GaussianRadius, double);

    protected:
        GraphcutSegmentationToSurfaceFilter();
        virtual ~GraphcutSegmentationToSurfaceFilter(){};

        bool m_UseMedian, m_UseThresholding;
        int m_MedianKernelSizeX, m_MedianKernelSizeY, m_MedianKernelSizeZ;

        bool m_UseGaussianSmoothing;
        double m_GaussianStandardDeviation, m_GaussianRadius;
    };
}
#endif //_MITKGRAPHCUTSEGMENTATIONTOSURFACEFILTER_h__
