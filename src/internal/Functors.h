#pragma once

#include <itkMath.h>
#include <itkRescaleIntensityImageFilter.h>
#include <math.h>

namespace Functors {
    using LinearTransform = itk::Functors::IntensityLinearTransform;

    // Exponential equation of the form: y=a*x^b+c
    template<class TPixel>
    class ExponentialTransform {
    public:
        using RealType = NumericTraits<TPixel>::RealType;
        RealType m_Factor = 1.0, m_Exponent = 1.0, m_Offset = 0.0;

        void SetOffset(RealType _a){
            m_Offset = _a;
        }
        void SetFactor(RealType _b){
            m_Factor = _b;
        }
        void SetExponent(RealType _c){
            m_Exponent = _c;
        }

        bool operator!=(const LinearTransform& _other) const {
            if(Math::NotExactlyEquals(m_Factor, _other.m_Factor)
               || Math::NotExactlyEquals(m_Exponent, _other.m_Exponent)
               || Math::NotExactlyEquals(m_Offset, _other.m_Offset))
            {
                return true;
            }
            return false;
        }

        bool operator==(const LinearTransform& _other) const {
            return !(*this != _other);
        }

        template<class TPixel>
        inline TPixel operator()(const TPixel &_x) const {
            return static_cast<TPixel>(m_Factor * pow(_x, m_Exponent) + m_Offset);
        }
    };
}