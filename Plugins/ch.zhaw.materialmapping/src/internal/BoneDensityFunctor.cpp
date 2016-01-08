#include <itkMath.h>

#include "BoneDensityFunctor.h"

bool BoneDensityFunctor::operator!=(const BoneDensityFunctor &_other) const {
    return !(*this == _other);
}

bool BoneDensityFunctor::operator==(const BoneDensityFunctor &_other) const {
    if (m_RhoCt == _other.m_RhoCt
        && m_RhoAsh == _other.m_RhoAsh
        && m_RhoApp == _other.m_RhoApp) {
        return true;
    }
    return false;
}

std::ostream &operator<<(std::ostream &_out, const BoneDensityFunctor &_f) {
    _out << "Bone density rho: " << std::endl;
    _out << _f.m_RhoCt << std::endl;
    _out << _f.m_RhoAsh << std::endl;
    _out << _f.m_RhoApp << std::endl;
    return _out;
}

void BoneDensityFunctor::SetRhoCt(BoneDensityParameters::RhoCt _rhoCt) {
    m_RhoCt = _rhoCt;
}

void BoneDensityFunctor::SetRhoAsh(BoneDensityParameters::RhoAsh _rhoAsh) {
    m_RhoAsh = _rhoAsh;
}

void BoneDensityFunctor::SetRhoApp(BoneDensityParameters::RhoApp _rhoApp) {
    m_RhoApp = _rhoApp;
}