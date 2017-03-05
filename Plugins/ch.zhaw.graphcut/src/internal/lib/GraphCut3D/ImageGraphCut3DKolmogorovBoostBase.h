/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef __ImageGraphCut3DKolmogorovBoostBase_h_
#define __ImageGraphCut3DKolmogorovBoostBase_h_

#include "ImageGraphCut3DFilter.h"
namespace itk{
	//! Base Class for the Kolmogorov maxflow & boost GraphCut solvers
	template<typename TInput, typename TForeground, typename TBackground, typename TOutput>
	class ImageGraphCut3DKolmogorovBoostBase : public ImageGraphCut3DFilter<TInput, TForeground, TBackground, TOutput>{
	public:
		// ITK related defaults
		typedef ImageGraphCut3DKolmogorovBoostBase Self;
		typedef ImageGraphCut3DFilter<TInput, TForeground, TBackground, TOutput> SuperClass;
		typedef SmartPointer<Self> Pointer;
		typedef SmartPointer<const Self> ConstPointer;

		itkTypeMacro(ImageGraphCut3DKolmogorovBoostBase, ImageGraphCut3DFilter);

		typedef typename SuperClass::InputImageType InputImageType;

		typedef typename SuperClass::ForegroundImageType ForegroundImageType;
		typedef typename SuperClass::BackgroundImageType BackgroundImageType;
		typedef typename SuperClass::OutputImageType OutputImageType;
		typedef typename SuperClass::IndexContainerType IndexContainerType;     // container for sinks / sources
		typedef typename SuperClass::WeightType WeightType;

		typedef typename SuperClass::ImageContainer ImageContainer;

        virtual void InitializeGraph(const ImageContainer) = 0;
		virtual void FillGraph(const ImageContainer, ProgressReporter &progress) override;

        virtual void CutGraph(ImageContainer, ProgressReporter &progress) override;
		virtual void addBidirectionalEdge(const unsigned int source, const unsigned int target, const float weight, const float reverseWeight) = 0;

        virtual void addTerminalEdges(const unsigned int node, const float sourceWeight, const float sinkWeight) = 0;

		// query the resulting segmentation group of a vertex.
		virtual int groupOf(const unsigned int vertex) const = 0;

        virtual int groupOfSource() = 0;
        virtual int groupOfSink() = 0;
        virtual unsigned int getNumberOfVertices() = 0;
        virtual unsigned int getNumberOfEdges()= 0;

    protected:
        ImageGraphCut3DKolmogorovBoostBase();

        virtual ~ImageGraphCut3DKolmogorovBoostBase();

	private:
        ImageGraphCut3DKolmogorovBoostBase(const Self &); // intentionally not implemented
		void operator=(const Self &); // intentionally not implemented
	};
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION

#include "ImageGraphCut3DKolmogorovBoostBase.hxx"

#endif

#endif //__ImageGraphCut3DKolmogorovBoostBase_h_
