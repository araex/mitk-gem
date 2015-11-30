#pragma once

#include "BoneDensityParameters.h"

class BoneDensityFunctor {
public:
    bool operator!=(const BoneDensityFunctor& _other) const;
    bool operator==(const BoneDensityFunctor& _other) const;

    template<class TPixel>
    inline TPixel operator()(const TPixel &_ct) const {
        auto rhoCt = _ct * m_RhoCt.slope + m_RhoCt.offset;
        auto rhoAsh = (rhoCt + m_RhoAsh.offset) / m_RhoAsh.divisor;
        auto rhoApp = rhoAsh / m_RhoApp.divisor;
        return static_cast<TPixel>(rhoApp);
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

protected:
    BoneDensityParameters::RhoCt m_RhoCt;
    BoneDensityParameters::RhoAsh m_RhoAsh;
    BoneDensityParameters::RhoApp m_RhoApp;
};