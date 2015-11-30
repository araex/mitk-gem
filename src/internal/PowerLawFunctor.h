#pragma once

#include <cmath>
#include <map>
#include <ostream>

#include "PowerLawParameters.h"

class PowerLawFunctor {
public:
    template<class TPixel>
    inline TPixel operator()(const TPixel &_rho) const {
        // TODO: verify the lookup works
        auto param = (std::find_if(m_ParamMap.cbegin(), m_ParamMap.cend(), std::bind2nd(std::greater_equal<TPixel>(), _rho)) - 1).second;
        return static_cast<TPixel>(std::pow(param.factor*_rho, param.exponent) + param.offset);
    }

    void AddPowerLaw(PowerLawParameters _p, double _upperBound);

    std::map<double, PowerLawParameters> m_ParamMap;
};

std::ostream& operator<<(std::ostream& _out, const PowerLawFunctor& _f);