/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef __ImageKolmogorovBoostFilter_h_
#define __ImageKolmogorovBoostFilter_h_

#include "ImageGraphCut3DFilter.h"
namespace itk{

	template<typename TInput, typename TForeground, typename TBackground, typename TOutput>
	class ImageKolmogorovBoostFilter : public ImageGraphCut3DFilter<TInput, TForeground, TBackground, TOutput>{
	public:
		// ITK related defaults
		typedef ImageKolmogorovBoostFilter Self;
		typedef ImageGraphCut3DFilter<TInput, TForeground, TBackground, TOutput> SuperClass;
		typedef SmartPointer<Self> Pointer;
		typedef SmartPointer<const Self> ConstPointer;

		itkTypeMacro(ImageKolmogorovBoostFilter, ImageToImageFilter);

		typedef typename SuperClass::InputImageType InputImageType;

		typedef typename SuperClass::ForegroundImageType ForegroundImageType;
		typedef typename SuperClass::BackgroundImageType BackgroundImageType;
		typedef typename SuperClass::OutputImageType OutputImageType;
		typedef typename SuperClass::IndexContainerType IndexContainerType;     // container for sinks / sources
		typedef typename SuperClass::WeightType WeightType;

		typedef typename SuperClass::ImageContainer ImageContainer;

        virtual void InitializeGraph(const ImageContainer) = 0;
		virtual void FillGraph(const ImageContainer, ProgressReporter &progress);

        virtual void CutGraph(ImageContainer, ProgressReporter &progress);
		virtual void addBidirectionalEdge(const unsigned int source, const unsigned int target, const float weight, const float reverseWeight) = 0;

        virtual void addTerminalEdges(const unsigned int node, const float sourceWeight, const float sinkWeight) = 0;

		// query the resulting segmentation group of a vertex.
		virtual int groupOf(const unsigned int vertex) const = 0;

        virtual int groupOfSource() = 0;
        virtual int groupOfSink() = 0;
        virtual unsigned int getNumberOfVertices() = 0;
        virtual unsigned int getNumberOfEdges()= 0;

    protected:
        ImageKolmogorovBoostFilter();

        virtual ~ImageKolmogorovBoostFilter();

	private:
        ImageKolmogorovBoostFilter(const Self &); // intentionally not implemented
		void operator=(const Self &); // intentionally not implemented
	};
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION

#include "ImageKolmogorovBoostFilter.hxx"

#endif

#endif //__ImageKolmogorovBoostFilter_h_
