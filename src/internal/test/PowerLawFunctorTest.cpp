#include "catch.hpp"

#include <cmath>

#include "../PowerLawParameters.h"
#include "../PowerLawFunctor.h"

TEST_CASE("PowerLawParameters"){
    SECTION("comparators"){
        PowerLawParameters p0(1, 2, 3);
        PowerLawParameters p1(1, 2, 3);
        PowerLawParameters p2(2, 2, 3);

        REQUIRE(p0 == p1);
        REQUIRE(p1 != p2);
        REQUIRE(!(p0 == p2));
    }
}

TEST_CASE("PowerLawFunctor"){
    PowerLawParameters p0(1, 4, 7);
    PowerLawParameters p1(2, 5, 8);
    PowerLawParameters p2(3, 6, 9);

    PowerLawFunctor functor;
    functor.AddPowerLaw(p0, 100);
    functor.AddPowerLaw(p1, 200);
    functor.AddPowerLaw(p2, 300);

    SECTION("functionality"){
        auto expectedFunctor = [](double x){
            if(x < 100){
                return 1*std::pow(x, 4) + 7;
            } else if (x >= 100 && x < 200){
                return 2*std::pow(x, 5) + 8;
            }
            return 3*std::pow(x, 6) + 9;
        };

        std::vector<double> numbers {-99999999, -100, 0, 99, 100, 101, 150, 199, 200, 201, 280, 299, 300, 301, 99999999};

        for(const auto &nr : numbers){
            REQUIRE(functor(nr) == expectedFunctor(nr));
        }
    }
}