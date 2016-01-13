#pragma once

#include <ostream>

#include "BoneDensityParameters.h"

/**
 * Functor combining rho_ct, rho_ash and rho_app
 */
class BoneDensityFunctor {
public:
    bool operator!=(const BoneDensityFunctor &_other) const;
    bool operator==(const BoneDensityFunctor &_other) const;

    /**
     * Evaluates bone density for a given ct value
     */
    template<class TPixel>
    inline double operator()(const TPixel &_ct) const {
        return (((_ct * m_RhoCt.slope + m_RhoCt.offset) + m_RhoAsh.offset) / m_RhoAsh.divisor) / m_RhoApp.divisor;
    }

    void SetRhoCt(BoneDensityParameters::RhoCt _rhoCt);
    void SetRhoAsh(BoneDensityParameters::RhoAsh _rhoAsh);
    void SetRhoApp(BoneDensityParameters::RhoApp _rhoApp);

    BoneDensityParameters::RhoCt m_RhoCt;
    BoneDensityParameters::RhoAsh m_RhoAsh;
    BoneDensityParameters::RhoApp m_RhoApp;
};

std::ostream &operator<<(std::ostream &_out, const BoneDensityFunctor &_f);