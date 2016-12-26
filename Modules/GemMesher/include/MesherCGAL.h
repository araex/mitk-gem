#pragma once

#include "IMesher.h"

namespace gem
{
    /*!
     * Implementation of a volume mesher using CGAL
     * @author Thomas Fitze
     */
    class MesherCGAL : public IMesher
    {
    protected:
        virtual void compute(void) override;
    };
}