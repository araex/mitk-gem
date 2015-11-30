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