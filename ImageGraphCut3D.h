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
template <typename TImage>
class ImageGraphCut3D
{
public:

  //YP constructor now in .hpp; was: ImageGraphCut3D(){}
  ImageGraphCut3D();

  /** This is the results image as uchar. */
  typedef itk::Image<unsigned char, 3> ResultImageType;

  /** This is a special type to keep track of the graph node labels. */
  typedef itk::Image<void*, 3> NodeImageType;

  /** The type of the histograms. */
  typedef itk::Statistics::Histogram< short,
          itk::Statistics::DenseFrequencyContainer2 > HistogramType;

  /** The type of a list of pixels/indexes. */
  typedef std::vector<itk::Index<3> > IndexContainer;

  /** Several initializations are done here. */
  void SetImage(TImage* const image);

  /** Get the image that we are segmenting. */
  TImage* GetImage();

  /** Create and cut the graph (The main driver function). */
  void PerformSegmentation();

  /** Return a list of the selected (via scribbling) pixels. */
  IndexContainer GetSources();
  IndexContainer GetSinks();

  /** Set the selected pixels. */
  void SetSources(const IndexContainer& sources);
  void SetSinks(const IndexContainer& sinks);

  /** Get the output of the segmentation. */
  ResultImageType::Pointer GetSegmentMask();

  /** Use the region term based on log histogram. */
  void UseRegionTermBasedOnHistogramOn();

  /** DON'T use the region term based on log histogram (default). */
  void UseRegionTermBasedOnHistogramOff();

  /** Set the weight between the regional and boundary terms. */
  void SetLambda(const float);

  /** Set sigma for boundary term. */
  void SetSigma(const float);

  /** Set the number of bins per dimension of the foreground and background histograms. */
  void SetNumberOfHistogramBins(const int);

  /** Deep copy from ITKHelpers. We could use the ImageDuplicator instead.*/
  void DeepCopy(TImage* input, TImage* output);
 

protected:

  /** A Kolmogorov graph object */
  GraphType* Graph;

  /** The output segmentation */
  ResultImageType::Pointer ResultingSegments;

  /** User specified foreground points */
  IndexContainer Sources;

  /** User specified background points */
  IndexContainer Sinks;

  /** Noise in boundary term */
  // YP was: float Sigma = 5.0f;
  float Sigma;

  /** Use the region term based on log histogram values */
  // YP was: bool UseRegionTermBasedOnHistogram = false;
  bool UseRegionTermBasedOnHistogram;

  /** The weighting between region and boundary terms */
  // YP was: float Lambda = 1.0f;
  float Lambda;

  /** The number of bins per dimension of the foreground and background histograms */
  // YP was: int NumberOfHistogramBins = 10;
  int NumberOfHistogramBins;

  /** An image which keeps tracks of the mapping between pixel index and graph node id */
  NodeImageType::Pointer NodeImage;

  // Typedefs
  typedef typename TImage::PixelType PixelType;
  typedef itk::Vector<PixelType, 1> ListSampleMeasurementVectorType;
  typedef itk::Statistics::ListSample<ListSampleMeasurementVectorType> SampleType;
  typedef itk::Statistics::SampleToHistogramFilter<SampleType, HistogramType> SampleToHistogramFilterType;

  /** Create the histograms from the users selections */
  void CreateSamples();

  /** Estimate the "camera noise" */
  double ComputeNoise();

  /** Create a Kolmogorov graph structure from the image and selections */
  void CreateGraph();

  /** Perform the s-t min cut */
  void CutGraph();

  /** The ITK data structure for storing the values that we will compute the histogram of. */
  typename SampleType::Pointer ForegroundSample;
  typename SampleType::Pointer BackgroundSample;

  /** The histograms. */
  // YP was:
  //const HistogramType* ForegroundHistogram = nullptr;
  //const HistogramType* BackgroundHistogram = nullptr;
  const HistogramType* ForegroundHistogram;
  const HistogramType* BackgroundHistogram;


  /** ITK filters to create histograms. */
  typename SampleToHistogramFilterType::Pointer ForegroundHistogramFilter;
  typename SampleToHistogramFilterType::Pointer BackgroundHistogramFilter;

  /** The image to be segmented */
  typename TImage::Pointer Image;

};

#include "ImageGraphCut3D.hpp"

#endif
