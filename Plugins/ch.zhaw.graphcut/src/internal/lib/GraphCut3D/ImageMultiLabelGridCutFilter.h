/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef __ImageMultiLabelGridCutFilter_h_
#define __ImageMultiLabelGridCutFilter_h_

#include "ImageMultiLabelGraphCut3DFilter.h"
#include "lib/gridcut/examples/include/AlphaExpansion/AlphaExpansion_3D_6C_MT.h"

namespace itk{

template<typename TInput, typename TMultiLabel, typename TOutput>
class ImageMultiLabelGridCutFilter : public ImageMultiLabelGraphCut3DFilter<TInput, TMultiLabel, TOutput>{
public:
	// ITK related defaults
	typedef ImageMultiLabelGridCutFilter Self;
	typedef ImageMultiLabelGraphCut3DFilter<TInput, TMultiLabel, TOutput> SuperClass;
	typedef SmartPointer<Self> Pointer;
	typedef SmartPointer<const Self> ConstPointer;
	itkNewMacro(Self);

	itkTypeMacro(ImageMultiLabelGridCutFilter, ImageMultiLabelGridCutFilter);

    typedef typename SuperClass::InputImageType InputImageType;

    typedef typename SuperClass::MultiLabelImageType MultiLabelImageType;
    typedef typename SuperClass::OutputImageType OutputImageType;
    typedef typename SuperClass::IndexContainerType IndexContainerType;     // container for sinks / sources
    typedef typename SuperClass::WeightType WeightType;

    typedef typename SuperClass::ImageContainer ImageContainer;
    typedef typename std::vector< std::vector<WeightType > > CapacityType;
    typedef AlphaExpansion_3D_6C_MT<typename TMultiLabel::PixelType, WeightType, WeightType> GraphType;

	virtual void FillGraph(const ImageContainer, ProgressReporter &progress) override;
    virtual void SolveGraph() override {
        m_Graph->perform(50);
    }
	virtual void CutGraph(ImageContainer, ProgressReporter &progress) override;


protected:

	ImageMultiLabelGridCutFilter();
    virtual ~ImageMultiLabelGridCutFilter();

    std::vector<unsigned int> mLabelIndex;
    std::vector<std::vector<WeightType>> mWeights; // store here the smooth cost weights because the alpha expansion class does not do it
    GraphType* m_Graph;

private:
	ImageMultiLabelGridCutFilter(const Self &); // intentionally not implemented
	void operator=(const Self &); // intentionally not implemented
};
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION

#include "ImageMultiLabelGridCutFilter.hxx"

#endif

#endif //__ImageMultiLabelGridCutFilter_h_
