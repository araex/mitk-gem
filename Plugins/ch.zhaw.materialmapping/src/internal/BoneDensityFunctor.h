#pragma once

#include <ostream>

#include "BoneDensityParameters.h"

class BoneDensityFunctor {
public:
    bool operator!=(const BoneDensityFunctor& _other) const;
    bool operator==(const BoneDensityFunctor& _other) const;

    template<class TPixel>
    inline TPixel operator()(const TPixel &_ct) const {
        return static_cast<TPixel>((((_ct * m_RhoCt.slope + m_RhoCt.offset) + m_RhoAsh.offset) / m_RhoAsh.divisor) / m_RhoApp.divisor);
    }

    void SetRhoCt(BoneDensityParameters::RhoCt _rhoCt){
        m_RhoCt = _rhoCt;
    }

    void SetRhoAsh(BoneDensityParameters::RhoAsh _rhoAsh){
        m_RhoAsh = _rhoAsh;
    }

    void SetRhoApp(BoneDensityParameters::RhoApp _rhoApp){
        m_RhoApp = _rhoApp;
    }

    BoneDensityParameters::RhoCt m_RhoCt;
    BoneDensityParameters::RhoAsh m_RhoAsh;
    BoneDensityParameters::RhoApp m_RhoApp;
};

std::ostream& operator<<(std::ostream& _out, const BoneDensityFunctor& _f);