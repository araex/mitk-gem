#include <itkMath.h>

#include "BoneDensityFunctor.h"

bool BoneDensityFunctor::operator!=(const BoneDensityFunctor &_other) const {
    return !(*this == _other);
}

bool BoneDensityFunctor::operator==(const BoneDensityFunctor &_other) const {
    if(m_RhoCt == _other.m_RhoCt
       && m_RhoAsh == _other.m_RhoAsh
       && m_RhoApp == _other.m_RhoApp)
    {
        return true;
    }
    return false;
}

std::ostream& operator<<(std::ostream& _out, const BoneDensityFunctor& _f) {
    _out << "Bone density rho: " << std::endl;
    _out << "rho_ct  = " << _f.m_RhoCt.slope << " * HU + " << _f.m_RhoCt.offset << std::endl;
    _out << "rho_ash = (rho_ct + " << _f.m_RhoAsh.offset << ") / " << _f.m_RhoAsh.divisor << std::endl;
    _out << "rho_app = rho_ash / " << _f.m_RhoApp.divisor;
    return _out;
}