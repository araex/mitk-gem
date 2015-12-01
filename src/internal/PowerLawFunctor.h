#pragma once

#include <cmath>
#include <map>
#include <ostream>

#include "PowerLawParameters.h"

class PowerLawFunctor {
public:
    template<class TPixel>
    inline TPixel operator()(const TPixel &_rho) const {
        auto it = m_ParamMap.upper_bound(_rho);

        if(it != cached_it){
            if(it == m_ParamMap.end()){ // value is out of bounds, fall back on the last defined one
                cached_param = &(*m_ParamMap.rbegin()).second;
            } else {
                cached_param = &(*it).second;
            }
            cached_it = it;
        }

        return static_cast<TPixel>(cached_param->factor*std::pow(_rho, cached_param->exponent) + cached_param->offset);
    }

    void AddPowerLaw(PowerLawParameters _p, double _upperBound);

    std::map<double, PowerLawParameters> m_ParamMap;

public:
    mutable std::map<double, PowerLawParameters>::const_iterator cached_it;
    mutable PowerLawParameters const *cached_param;
};

std::ostream& operator<<(std::ostream& _out, const PowerLawFunctor& _f);