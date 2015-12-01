#include "catch.hpp"

#include "../PowerLawWidget.h"
#include "../PowerLawWidgetManager.h"

TEST_CASE("PowerLawWidget"){
    PowerLawWidget w0, w1;
    w0.m_Factor->setValue(1);
    w0.m_Exponent->setValue(2);
    w0.m_Offset->setValue(3);

    w1.m_Factor->setValue(4);
    w1.m_Exponent->setValue(5);
    w1.m_Offset->setValue(6);

    SECTION("constructing parameters from gui"){
        PowerLawParameters expectedP0(1,2,3);
        PowerLawParameters expectedP1(4,5,6);
        auto p0 = w0.getPowerLawParameters();
        auto p1 = w1.getPowerLawParameters();
        REQUIRE(p0==expectedP0);
        REQUIRE(p1==expectedP1);
    }

    SECTION("signals between 2 widgets"){
        w0.connect(&w1);
        w0.setMax(50);
        REQUIRE(w0.getMax() == w1.getMin());
        w1.setMax(75);
        REQUIRE(w0.getMax() == w1.getMin());
    }
}

TEST_CASE("PowerLawWidgetManager"){
    auto parent = new QWidget;
    auto layout = new QHBoxLayout;
    parent->setLayout(layout);

    PowerLawWidgetManager manager(parent);

    SECTION("check signals"){
        auto w0 = manager.addPowerLaw();
        auto w1 = manager.addPowerLaw();

        w0->setMax(50);
        REQUIRE(w0->getMax() == w1->getMin());
        w1->setMax(75);
        REQUIRE(w0->getMax() == w1->getMin());
    }

    SECTION("range adaption on add"){
        // TODO: these should be a field in the widgets
        auto minValue = -9999;
        auto maxValue = 9999;

        auto w0 = manager.addPowerLaw();
        REQUIRE(w0->getMin() == minValue);
        REQUIRE(w0->getMax() == minValue);

        auto w1 = manager.addPowerLaw();
        REQUIRE(w0->getMin() == minValue);
        REQUIRE(w0->getMax() == w1->getMin());
        REQUIRE(w0->getMax() != minValue);
        REQUIRE(w1->getMax() == minValue);
    }
}