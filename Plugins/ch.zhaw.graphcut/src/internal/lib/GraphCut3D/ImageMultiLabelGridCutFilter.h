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
        m_Graph->compute_maxflow();
    }
	virtual void CutGraph(ImageContainer, ProgressReporter &progress) override;

    void SetCapacities(const float* cap_s,
                       const float* cap_t,
                       const float* cap_lee,
                       const float* cap_gee,
                       const float* cap_ele,
                       const float* cap_ege,
                       const float* cap_eel,
                       const float* cap_eeg)
    {
        m_Graph->set_caps(cap_s, cap_t, cap_lee, cap_gee, cap_ele, cap_ege, cap_eel, cap_eeg);
    }

    virtual inline int groupOf(const int x, const int y, const int z) const{
        return (short) m_Graph->get_segment(m_Graph->node_id(x, y, z));
    }

protected:

	ImageMultiLabelGridCutFilter();
    virtual ~ImageMultiLabelGridCutFilter();

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
