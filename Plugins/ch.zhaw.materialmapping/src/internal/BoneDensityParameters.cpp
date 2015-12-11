#include <limits>
#include <cmath>

#include "BoneDensityParameters.h"

namespace BoneDensityParameters {
    RhoCt::RhoCt() : RhoCt(1, 0){}
    RhoCt::RhoCt(double _s, double _o) : slope(_s), offset(_o){}

    RhoAsh::RhoAsh() : RhoAsh(0, 1){}
    RhoAsh::RhoAsh(double _o, double _d) : offset(_o), divisor(_d){}

    RhoApp::RhoApp() : RhoApp(1){}
    RhoApp::RhoApp(double _d) : divisor(_d){}

    bool almostEqual(double a, double b) {
        return std::abs(a - b) < 0.0000001;
    }

    bool operator==(const RhoCt& lhs, const RhoCt& rhs) {
        return almostEqual(lhs.slope, rhs.slope) && almostEqual(lhs.offset, rhs.offset);
    }

    bool operator==(const RhoAsh& lhs, const RhoAsh& rhs) {
        return almostEqual(lhs.offset, rhs.offset) && almostEqual(lhs.divisor, rhs.divisor);
    }

    bool operator==(const RhoApp& lhs, const RhoApp& rhs) {
        return almostEqual(lhs.divisor, rhs.divisor);
    }

    bool operator!=(const RhoCt& lhs, const RhoCt& rhs) {
        return !(lhs == rhs);
    }

    bool operator!=(const RhoAsh& lhs, const RhoAsh& rhs) {
        return !(lhs == rhs);
    }

    bool operator!=(const RhoApp& lhs, const RhoApp& rhs) {
        return !(lhs == rhs);
    }
}