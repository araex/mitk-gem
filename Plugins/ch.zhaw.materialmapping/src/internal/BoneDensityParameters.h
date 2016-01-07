#pragma once

#include <ostream>

namespace BoneDensityParameters {
    // rho_ct = slope * HU + offset
    struct RhoCt{
        RhoCt();
        RhoCt(double _slope, double _offset);
        double slope, offset;
    };

    // rho_ash = (rho_ct + offset) / divisor
    struct RhoAsh{
        RhoAsh();
        RhoAsh(double _offset, double _divisor);
        double offset, divisor;
    };

    // rho_app = rho_ash / divisor
    struct RhoApp{
        RhoApp();
        RhoApp(double _divisor);
        double divisor;
    };

    // comparators
    bool operator==(const RhoCt& lhs, const RhoCt& rhs);
    bool operator==(const RhoAsh& lhs, const RhoAsh& rhs);
    bool operator==(const RhoApp& lhs, const RhoApp& rhs);
    bool operator!=(const RhoCt& lhs, const RhoCt& rhs);
    bool operator!=(const RhoAsh& lhs, const RhoAsh& rhs);
    bool operator!=(const RhoApp& lhs, const RhoApp& rhs);

    // to string
    std::ostream& operator<<(std::ostream& _out, const RhoCt&);
    std::ostream& operator<<(std::ostream& _out, const RhoAsh&);
    std::ostream& operator<<(std::ostream& _out, const RhoApp&);
};