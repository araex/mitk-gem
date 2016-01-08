#pragma once

#include <cmath>
#include <map>
#include <ostream>

#include "PowerLawParameters.h"

/**
 * Functor that maintains a map of power laws and their definition intervals.
 */
class PowerLawFunctor {
public:
    /**
     * Selects the correct power law for the given rho and applies it.
     */
    template<class TPixel>
    inline TPixel operator()(const TPixel &_rho) const {
        auto it = m_ParamMap.upper_bound(_rho);

        if (it != cached_it) {  // the PowerLawParameter comparator is rather slow, so we'll use the iterator
            if (it == m_ParamMap.end()) { // value is out of bounds, fall back on the last defined one
                cached_param = &(*m_ParamMap.rbegin()).second;
            } else {
                cached_param = &(*it).second;
            }
            cached_it = it;
        }

        return static_cast<TPixel>(cached_param->factor * std::pow(_rho, cached_param->exponent) + cached_param->offset);
    }

    /**
     * Add a power law with an upper bound (rho < upper bound). The lower bound to a power law is either the upper bound
     * of the previous power law or the minimal value.
     *
     * Note that the functor does not perform any sanity checks! If 2 power laws with the same upper bound are added,
     * the functor will always select the one added first!
     */
    void AddPowerLaw(PowerLawParameters _p, double _upperBound);

    std::map<double, PowerLawParameters> m_ParamMap;

public:
    /**
     * Currently, the functor is applied in a way like this (pseudo code):
     * for(auto &point; ugrid->GetPoints()) {
     *   valCT = img[point];
     *   valRho = boneDensityFunctor(valCT);
     *   valEMorgan = powerLawFunctor(valRho);
     * }
     * Since the number of power laws is usually rather small and access order of points tends to be favorable, caching
     * the power law parameters turns out to be a surprisingly big performance boost.
     */
    mutable std::map<double, PowerLawParameters>::const_iterator cached_it;
    mutable PowerLawParameters const *cached_param;
};

std::ostream &operator<<(std::ostream &_out, const PowerLawFunctor &_f);