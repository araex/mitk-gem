#ifndef __GraphCut_h__
#define __GraphCut_h__

#include "lib/gridcut/config.h"
#ifdef GRIDCUT_LIBRARY_AVAILABLE
#include "ImageGridCutFilter.hxx"
#else
#include "ImageGraphCut3DKolmogorovFilter.hxx"
#endif

namespace GraphCut
{
    template<typename TInput, typename TForeground, typename TBackground, typename TOutput>
    #ifdef GRIDCUT_LIBRARY_AVAILABLE
        using FilterType = itk::ImageGridCutFilter<TInput, TForeground, TBackground, TOutput>;
    #else
        using FilterType = itk::ImageGraphCut3DKolmogorovFilter<TInput, TForeground, TBackground, TOutput>;
    #endif // GRIDCUT_LIBRARY_AVAILABLE
}

#endif //__GraphCut_h__
