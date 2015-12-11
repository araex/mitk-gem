#include <cmath>
#include "PowerLawParameters.h"

PowerLawParameters::PowerLawParameters() : PowerLawParameters(1, 1, 0){}
PowerLawParameters::PowerLawParameters(double _factor, double _exponent, double _offset)
        : factor(_factor), exponent(_exponent), offset(_offset) { }

bool almostEqual(double a, double b) {
    return std::abs(a - b) < 0.0000001;
}

bool operator==(const PowerLawParameters& lhs, const PowerLawParameters& rhs) {
    return almostEqual(lhs.factor, rhs.factor) && almostEqual(lhs.exponent, rhs.exponent) && almostEqual(lhs.offset, rhs.offset);
}

bool operator!=(const PowerLawParameters& lhs, const PowerLawParameters& rhs) {
    return !(lhs == rhs);
}