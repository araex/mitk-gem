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
template<typename TImage>
class ImageGraphCut3D {
public:
    // typedefs
    typedef itk::Image<unsigned char, 3> ResultImageType;
    typedef itk::Statistics::Histogram<short, itk::Statistics::DenseFrequencyContainer2> HistogramType;
    typedef itk::Image<void *, 3> NodeImageType; // graph node labels
    typedef std::vector<itk::Index<3> > IndexContainer; // container for sinks / sources
    typedef typename TImage::PixelType PixelType;

    ImageGraphCut3D();


    // functions
    void PerformSegmentation();
    void SetImage(TImage *const image); // TODO: more than just a setter

    // setters
    void SetLambda(const double lambda){
        m_Lambda = lambda;
    }
    void SetSigma(const double sigma){
        m_Sigma = sigma;
    }
    void SetNumberOfHistogramBins(const int bins){
        m_NumberOfHistogramBins = bins;
    }
    void SetSources(const IndexContainer &sources){
        m_Sources = sources;
    }
    void SetSinks(const IndexContainer &sinks){
        m_Sinks = sinks;
    }

    // getters
    TImage *GetImage(){
        return m_Image;
    }
    ResultImageType::Pointer GetSegmentMask(){
        return m_ResultingSegments;
    }
    IndexContainer GetSources(){
        return m_Sources;
    }
    IndexContainer GetSinks(){
        return m_Sinks;
    }

    // Enums used to specify Boundary term direction
    typedef enum {
        NoDirection, BrightDark, DarkBright
    } BoundaryDirectionType;
    void SetBoundaryDirectionTypeToNoDirection(){
        m_BoundaryDirectionType = NoDirection;
    }
    void SetBoundaryDirectionTypeToBrightDark(){
        m_BoundaryDirectionType = BrightDark;
    }
    void SetBoundaryDirectionTypeToDarkBright(){
        m_BoundaryDirectionType = DarkBright;
    }

protected:
    // Typedefs
    typedef itk::Vector<PixelType, 1> ListSampleMeasurementVectorType;
    typedef itk::Statistics::ListSample<ListSampleMeasurementVectorType> SampleType;
    typedef itk::Statistics::SampleToHistogramFilter<SampleType, HistogramType> SampleToHistogramFilterType;

    // members
    GraphType                   *m_Graph;               // kolmogorov graph object
    typename TImage::Pointer    m_Image;                // input image
    NodeImageType::Pointer      m_NodeImage;            // mapping pixel index -> graph node id
    ResultImageType::Pointer    m_ResultingSegments;    // resulting segmentation
    IndexContainer              m_Sources;              // foreground pixel indices
    IndexContainer              m_Sinks;                // background pixel indices

    // parameters
    double m_Sigma;                         // noise in boundary term
    double m_Lambda;                        // weighting between region and boundary terms
    int m_NumberOfHistogramBins;            // number of bins per dimension of the foreground and background histograms
    BoundaryDirectionType m_BoundaryDirectionType; // Direction of the Boundary term

    // member functions
    void CreateSamples();       // create histograms from the users selections
    double ComputeNoise();      // Estimate the "camera noise"
    void CreateGraph();         // Create a Kolmogorov graph structure from the image and selections
    void CutGraph();            // Perform the s-t min cut

    /** The ITK data structure for storing the values that we will compute the histogram of. */
    typename SampleType::Pointer m_ForegroundSample;
    typename SampleType::Pointer m_BackgroundSample;

    /** The histograms. */
    const HistogramType *m_ForegroundHistogram;
    const HistogramType *m_BackgroundHistogram;


    /** ITK filters to create histograms. */
    typename SampleToHistogramFilterType::Pointer m_ForegroundHistogramFilter;
    typename SampleToHistogramFilterType::Pointer m_BackgroundHistogramFilter;

private:
    // in-class initializations of dependent types is only possible in >=C++11.
    // to ensure backward compatibility, the initialization is done in the constructors initialization list
    const typename ResultImageType::PixelType RESULT_FOREGROUND_PIXEL_VALUE;
    const typename ResultImageType::PixelType RESULT_BACKGROUND_PIXEL_VALUE;
    bool m_LogToStd;

};

#include "ImageGraphCut3D.hpp"

#endif
