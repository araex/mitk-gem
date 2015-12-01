#include "BoneDensityParameters.h"

namespace BoneDensityParameters {
    RhoCt::RhoCt() : RhoCt(1, 0){}
    RhoCt::RhoCt(double _s, double _o) : slope(_s), offset(_o){}

    RhoAsh::RhoAsh() : RhoAsh(0, 1){}
    RhoAsh::RhoAsh(double _o, double _d) : offset(_o), divisor(_d){}

    RhoApp::RhoApp() : RhoApp(1){}
    RhoApp::RhoApp(double _d) : divisor(_d){}

    bool operator==(const RhoCt& lhs, const RhoCt& rhs) {
        return (lhs.slope == rhs.slope) && (lhs.offset == rhs.offset);
    }

    bool operator==(const RhoAsh& lhs, const RhoAsh& rhs) {
        return (lhs.offset == rhs.offset) && (lhs.divisor == rhs.divisor);
    }

    bool operator==(const RhoApp& lhs, const RhoApp& rhs) {
        return (lhs.divisor == rhs.divisor);
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