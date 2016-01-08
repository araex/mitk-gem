#include "PowerLawFunctor.h"

void PowerLawFunctor::AddPowerLaw(PowerLawParameters _p, double _upperBound) {
    m_ParamMap.insert(std::make_pair(_upperBound, _p));
}

std::ostream &operator<<(std::ostream &_out, const PowerLawFunctor &_f) {
    _out << "Power laws: " << std::endl;
    for (const auto &pair : _f.m_ParamMap) {
        _out << "[" << pair.first << "] E = " << pair.second.factor << " * rho ^ " << pair.second.exponent << " + " << pair.second.offset << std::endl;
    }
    return _out;
}