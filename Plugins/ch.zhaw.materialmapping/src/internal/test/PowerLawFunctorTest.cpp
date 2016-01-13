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
    PowerLawParameters p0(1, 1, 7);
    PowerLawParameters p1(6850, 1.49, 8);
    PowerLawParameters p2(3, 6, 9);

    PowerLawFunctor functor;
    functor.AddPowerLaw(p0, 0);
    functor.AddPowerLaw(p1, 200);
    functor.AddPowerLaw(p2, 300);

    SECTION("functionality"){
        auto expectedFunctor = [](double x){
            if(x < 0){
                return 1 * std::pow(x, 1) + 7;
            } else if (x >= 0 && x < 200){
                return 6850*std::pow(x, 1.49) + 8;
            }
            return 3*std::pow(x, 6) + 9;
        };

        std::vector<double> numbers {-99999999, -100, -.00001, 0, 0.0001, 99, 100, 101, 150, 199, 200, 201, 280, 299, 300, 301, 99999999};

        for(const auto &nr : numbers){
            auto result = functor(nr);
            REQUIRE( result == expectedFunctor(nr));
        }
    }
}