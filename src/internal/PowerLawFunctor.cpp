#include "PowerLawFunctor.h"

void PowerLawFunctor::AddPowerLaw(PowerLawParameters _p, double _upperBound) {
    m_ParamMap.insert(std::make_pair(_upperBound, _p));
}