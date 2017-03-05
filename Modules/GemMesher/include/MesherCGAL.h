#pragma once

#include "IMesher.h"

namespace gem
{
    /*!
     * Implementation of a volume mesher using CGAL
     * @author Thomas Fitze
     */
    class GemMesher_EXPORT MesherCGAL : public IMesher
    {
    public:
        struct SOptions
        {
            float fEdgeSize = 2.f;
            float fRadiusEdgeRatio =  3.f;
        };

        MesherCGAL(SOptions opt) : m_options(opt) {};

    protected:
        virtual void compute(void) override;

    private:
        SOptions m_options;
    };
}