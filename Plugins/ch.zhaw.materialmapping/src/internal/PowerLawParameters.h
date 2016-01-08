#pragma once

/**
 * E = factor * rho ^ exponent + offset
 */
struct PowerLawParameters {
    PowerLawParameters();
    PowerLawParameters(double _factor, double _exponent, double _offset);
    double factor, exponent, offset;
};

bool operator==(const PowerLawParameters &lhs, const PowerLawParameters &rhs);
bool operator!=(const PowerLawParameters &lhs, const PowerLawParameters &rhs);