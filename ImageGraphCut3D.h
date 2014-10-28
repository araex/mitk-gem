/*

Author: Yves Pauchard, yves.pauchard@zhaw.ch
Date: Nov 5, 2013
Description: Adaptation of the 2D RGB version by David Doria to 3D single value images.


Based on ImageGraphCut.h:
Copyright (C) 2012 David Doria, daviddoria@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ImageGraphCut3D_H
#define ImageGraphCut3D_H


// ITK
#include "itkImage.h"
#include "itkSampleToHistogramFilter.h"
#include "itkHistogram.h"
#include "itkListSample.h"

// STL
#include <vector>

// Kolmogorov's code
#include "Kolmogorov/graph.h"

typedef Graph GraphType;

/** Perform graph cut based segmentation on a 3D image. Image pixels can contain only
* one component, i.e. grayscale.
*/
template<typename TImage, typename TForeground, typename TBackground, typename TOutput>
class ImageGraphCut3D {
public:
    // image types
    typedef TImage InputImageType;
    typedef TForeground ForegroundImageType;
    typedef TBackground BackgroundImageType;
    typedef itk::Image<void *, 3> NodeImageType; // graph node mappings
    typedef TOutput OutputImageType;

    typedef itk::Statistics::Histogram<short, itk::Statistics::DenseFrequencyContainer2> HistogramType;
    typedef std::vector<itk::Index<3> > IndexContainerType;     // container for sinks / sources
    typedef enum {
        NoDirection, BrightDark, DarkBright
    } BoundaryDirectionType;

    ImageGraphCut3D();
    void PerformSegmentation();

    // setters
    void SetBoundaryDirectionTypeToNoDirection(){
        m_BoundaryDirectionType = NoDirection;
    }
    void SetBoundaryDirectionTypeToBrightDark(){
        m_BoundaryDirectionType = BrightDark;
    }
    void SetBoundaryDirectionTypeToDarkBright(){
        m_BoundaryDirectionType = DarkBright;
    }
    void SetInputImage(InputImageType *const image){
        m_InputImage = image;
    }
    void SetLambda(const double lambda){
        m_Lambda = lambda;
    }
    void SetSigma(const double sigma){
        m_Sigma = sigma;
    }
    void SetNumberOfHistogramBins(const int bins){
        m_NumberOfHistogramBins = bins;
    }
    void SetSources(const IndexContainerType &sources){
        m_Sources = sources;
    }
    void SetSinks(const IndexContainerType &sinks){
        m_Sinks = sinks;
    }

    // getters
    InputImageType *GetInputImage(){
        return m_InputImage;
    }
    typename OutputImageType::Pointer GetSegmentMask(){
        return m_ResultMask;
    }
    IndexContainerType GetSources(){
        return m_Sources;
    }
    IndexContainerType GetSinks(){
        return m_Sinks;
    }


protected:
    typedef itk::Vector<typename InputImageType::PixelType, 1> ListSampleMeasurementVectorType;
    typedef itk::Statistics::ListSample<ListSampleMeasurementVectorType> SampleType;
    typedef itk::Statistics::SampleToHistogramFilter<SampleType, HistogramType> SampleToHistogramFilterType;

    // members
    typename InputImageType::Pointer    m_InputImage;
    typename TForeground::Pointer       m_ForegroundImage;
    typename TBackground::Pointer       m_BackgroundImage;
    typename OutputImageType::Pointer   m_ResultMask;
    NodeImageType::Pointer              m_NodeImage;            // mapping pixel index -> graph node id
    GraphType                           *m_Graph;               // kolmogorov graph object
    IndexContainerType                  m_Sources;              // foreground pixel indices
    IndexContainerType                  m_Sinks;                // background pixel indices

    // histogram related
    typename SampleType::Pointer m_ForegroundSample;
    typename SampleType::Pointer m_BackgroundSample;
    const HistogramType *m_ForegroundHistogram;
    const HistogramType *m_BackgroundHistogram;
    typename SampleToHistogramFilterType::Pointer m_ForegroundHistogramFilter;
    typename SampleToHistogramFilterType::Pointer m_BackgroundHistogramFilter;

    // constants
    const typename OutputImageType::PixelType RESULT_FOREGROUND_PIXEL_VALUE;
    const typename OutputImageType::PixelType RESULT_BACKGROUND_PIXEL_VALUE;

    // parameters
    double                 m_Sigma;                     // noise in boundary term
    double                 m_Lambda;                    // weighting of hard constraints
    int                    m_NumberOfHistogramBins;     // bins per dimension of foreground and background histograms
    BoundaryDirectionType  m_BoundaryDirectionType;

    // member functions
    double ComputeNoise();      // Estimate the "camera noise"
    void CreateSamples();       // create histograms from the users selections
    void CreateGraph();         // Create a Kolmogorov graph structure from the image and selections
    void CutGraph();            // Perform the s-t min cut
};

#include "ImageGraphCut3D.hpp"

#endif
