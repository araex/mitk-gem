/**
 *  Image GraphCut 3D Segmentation
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef __ImageGridCutFilter_h_
#define __ImageGridCutFilter_h_

#include "ImageGraphCut3DFilter.h"
#include "lib/gridcut/include/GridCut/GridGraph_3D_6C_MT.h"

namespace itk{

template<typename TInput, typename TForeground, typename TBackground, typename TOutput>
class ImageGridCutFilter : public ImageGraphCut3DFilter<TInput, TForeground, TBackground, TOutput>{
public:
	// ITK related defaults
	typedef ImageGridCutFilter Self;
	typedef ImageGraphCut3DFilter<TInput, TForeground, TBackground, TOutput> SuperClass;
	typedef SmartPointer<Self> Pointer;
	typedef SmartPointer<const Self> ConstPointer;
	itkNewMacro(Self);

	itkTypeMacro(ImageGridCutFilter, ImageGraphCut3DFilter);

    typedef typename SuperClass::InputImageType InputImageType;

    typedef typename SuperClass::ForegroundImageType ForegroundImageType;
    typedef typename SuperClass::BackgroundImageType BackgroundImageType;
    typedef typename SuperClass::OutputImageType OutputImageType;
    typedef typename SuperClass::IndexContainerType IndexContainerType;     // container for sinks / sources
    typedef typename SuperClass::WeightType WeightType;

    typedef typename SuperClass::ImageContainer ImageContainer;
    typedef typename std::vector< std::vector<WeightType > > CapacityType;
    typedef GridGraph_3D_6C_MT<WeightType,WeightType,WeightType> GraphType;

	virtual void FillGraph(const ImageContainer, ProgressReporter &progress);
    virtual void SolveGraph(){
        m_Graph->compute_maxflow();
    }
	virtual void CutGraph(ImageContainer, ProgressReporter &progress);

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

    virtual int groupOfSource() const{
        return 0;
    }

    virtual int groupOfSink() const{
        return 1;
    }

protected:

	ImageGridCutFilter();
    virtual ~ImageGridCutFilter();

    GraphType* m_Graph;

private:
	ImageGridCutFilter(const Self &); // intentionally not implemented
	void operator=(const Self &); // intentionally not implemented
};
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION

#include "ImageGridCutFilter.hxx"

#endif

#endif //__ImageGridCutFilter_h_
