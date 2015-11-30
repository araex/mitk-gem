#pragma once

namespace BoneDensityParameters {
    struct RhoCt{
        RhoCt();
        RhoCt(double _slope, double _offset);
        double slope, offset;
    };
    struct RhoAsh{
        RhoAsh();
        RhoAsh(double _offset, double _divisor);
        double offset, divisor;
    };
    struct RhoApp{
        RhoApp();
        RhoApp(double _divisor);
        double divisor;
    };

    bool operator==(const RhoCt& lhs, const RhoCt& rhs);
    bool operator==(const RhoAsh& lhs, const RhoAsh& rhs);
    bool operator==(const RhoApp& lhs, const RhoApp& rhs);
};