#ifndef __GraphCut_h__
#define __GraphCut_h__

#include "lib/gridcut/config.h"
#ifdef GRIDCUT_LIBRARY_AVAILABLE
#include "ImageMultiLabelGridCutFilter.h"
#endif

namespace GraphCut
{
    template<typename TInput, typename TMultiLabel, typename TOutput>
    #ifdef GRIDCUT_LIBRARY_AVAILABLE
        using FilterType = itk::ImageMultiLabelGridCutFilter<TInput, TMultiLabel, TOutput>;
    #endif // GRIDCUT_LIBRARY_AVAILABLE
}

#endif //__GraphCut_h__
