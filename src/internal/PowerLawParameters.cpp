#include "PowerLawParameters.h"

PowerLawParameters::PowerLawParameters() : PowerLawParameters(1, 1, 0){}
PowerLawParameters::PowerLawParameters(double _factor, double _exponent, double _offset)
        : factor(_factor), exponent(_exponent), offset(_offset) { }

bool operator==(const PowerLawParameters& lhs, const PowerLawParameters& rhs) {
    return (lhs.factor == rhs.factor) && (lhs.exponent == rhs.exponent) && (lhs.offset == rhs.offset);
}