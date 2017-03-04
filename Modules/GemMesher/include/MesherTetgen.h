#pragma once

#include "IMesher.h"
#include "tetgen.h"

namespace gem
{
    /*!
     * Implementation of a volume mesher using tetgen
     * @author Thomas Fitze
     */
    class GemMesher_EXPORT MesherTetgen : public IMesher
    {
    public:
        /*!
         * Constructor
         * @param rParameters   Options passed to tetgen
         */
        MesherTetgen(tetgenbehavior &&rParameters);

    protected:
        virtual void compute(void) override;

    private:
        tetgenbehavior m_Options;   //!< Options passed to tetgen
    };
}