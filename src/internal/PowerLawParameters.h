#pragma once

struct PowerLawParameters {
    PowerLawParameters();
    PowerLawParameters(double _factor, double _exponent, double _offset);
    double factor, exponent, offset;
};