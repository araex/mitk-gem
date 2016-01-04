#include "catch.hpp"

#include "../BoneDensityFunctor.h"
#include "../GuiHelpers.h"
#include "../CalibrationDataModel.h"
#include "../MaterialMappingView.h"

TEST_CASE("BoneDensityFunctor"){
    BoneDensityParameters::RhoCt rhoCt(2, 10); // f(x) = 2x + 10
    BoneDensityParameters::RhoAsh rhoAsh(5, 2); // g(f(x)) = (x + 5) / 2
    BoneDensityParameters::RhoApp rhoApp(4); // h(g(f(x))) = x / 4
    BoneDensityFunctor functor;
    functor.SetRhoCt(rhoCt);
    functor.SetRhoAsh(rhoAsh);
    functor.SetRhoApp(rhoApp);

    SECTION("functionality"){
        auto expectedFunctor = [](double x){
            auto ctVal = 2 * x + 10;
            auto ashVal = (ctVal + 5) / 2;
            auto appVal = ashVal / 4;
            return appVal;
        };

        std::vector<double> numbers {0, 0.0001, -0.0001, 1000, -1000};

        for(const auto &nr : numbers){
            REQUIRE(functor(nr) == expectedFunctor(nr));
        }
    }

    SECTION("comparators"){
        BoneDensityFunctor functor2; // same parameters => equal
        functor2.SetRhoCt(rhoCt);
        functor2.SetRhoAsh(rhoAsh);
        functor2.SetRhoApp(rhoApp);

        BoneDensityFunctor functor3;
        BoneDensityParameters::RhoCt rhoCt3(3, 10); // RhoCT changed
        functor3.SetRhoCt(rhoCt3);
        functor3.SetRhoAsh(rhoAsh);
        functor3.SetRhoApp(rhoApp);

        REQUIRE(functor == functor2);
        REQUIRE(functor2 != functor3);
        REQUIRE(!(functor == functor3));
    }
}

TEST_CASE("BoneDensityParameters"){
    SECTION("comparators RhoCt"){
        BoneDensityParameters::RhoCt rhoCt0(2, 10);
        BoneDensityParameters::RhoCt rhoCt1(2, 10);
        BoneDensityParameters::RhoCt rhoCt2(3, 10);

        REQUIRE(rhoCt0 == rhoCt1);
        REQUIRE(rhoCt1 != rhoCt2);
        REQUIRE(!(rhoCt0 == rhoCt2));
    }

    SECTION("comparators RhoAsh"){
        BoneDensityParameters::RhoAsh rhoAsh0(5, 2);
        BoneDensityParameters::RhoAsh rhoAsh1(5, 2);
        BoneDensityParameters::RhoAsh rhoAsh2(6, 2);

        REQUIRE(rhoAsh0 == rhoAsh1);
        REQUIRE(rhoAsh1 != rhoAsh2);
        REQUIRE(!(rhoAsh0 == rhoAsh2));
    }

    SECTION("comparators RhoApp"){
        BoneDensityParameters::RhoApp rhoApp0(4);
        BoneDensityParameters::RhoApp rhoApp1(4);
        BoneDensityParameters::RhoApp rhoApp2(5);

        REQUIRE(rhoApp0 == rhoApp1);
        REQUIRE(rhoApp1 != rhoApp2);
        REQUIRE(!(rhoApp0 == rhoApp2));
    }
}

TEST_CASE("BoneDensityGui"){
    CalibrationDataModel dataModel;

    BoneDensityParameters::RhoCt rhoCt(2, 10);
    BoneDensityParameters::RhoAsh rhoAsh(5, 2);
    BoneDensityParameters::RhoApp rhoApp(4);
    BoneDensityFunctor functor;
    functor.SetRhoCt(rhoCt);
    functor.SetRhoAsh(rhoAsh);
    functor.SetRhoApp(rhoApp);

    Ui::MaterialMappingViewControls *gui = MaterialMappingView::controls;
    REQUIRE(gui != nullptr);

    gui->rhoAshCheckBox->setChecked(true);
    gui->rhoAshOffsetSpinBox->setValue(5);
    gui->rhoAshDivisorSpinBox->setValue(2);
    gui->rhoAppCheckBox->setChecked(true);
    gui->rhoAppDivisorSpinBox->setValue(4);

    SECTION("data model line fitting grams"){
        dataModel.setUnit(CalibrationDataModel::Unit::gHA_cm3);
        dataModel.appendRow(0, 10);
        dataModel.appendRow(-5, 0);
        auto createdRhoCt = dataModel.getFittedLine();
        REQUIRE(createdRhoCt == rhoCt);
    }

    SECTION("data model mg conversion"){
        dataModel.setUnit(CalibrationDataModel::Unit::mgHA_cm3);
        dataModel.appendRow(0, 10*1000.0);
        dataModel.appendRow(-5, 0);
        auto createdRhoCt = dataModel.getFittedLine();
        REQUIRE(createdRhoCt == rhoCt);
    }

    SECTION("functor creation"){
        dataModel.setUnit(CalibrationDataModel::Unit::gHA_cm3);
        dataModel.appendRow(0, 10);
        dataModel.appendRow(-5, 0);
        auto createdFunctor = gui::createDensityFunctorFromGui(*gui, dataModel);
        REQUIRE(createdFunctor == functor);
    }
}

TEST_CASE("BoneDensity unit sanity check"){
    CalibrationDataModel dataModel;

    SECTION("value range too high"){
        dataModel.setUnit(CalibrationDataModel::Unit::gHA_cm3);
        dataModel.appendRow(100, 120);
        dataModel.appendRow(120, 150);
        REQUIRE(dataModel.hasExpectedValueRange() == false);
    }

    SECTION("value range too low"){
        dataModel.setUnit(CalibrationDataModel::Unit::mgHA_cm3);
        dataModel.appendRow(100, 0.12);
        dataModel.appendRow(120, 0.15);
        REQUIRE(dataModel.hasExpectedValueRange() == false);
    }

    SECTION("correct value range mg"){
        dataModel.setUnit(CalibrationDataModel::Unit::mgHA_cm3);
        dataModel.appendRow(100, 120);
        dataModel.appendRow(120, 150);
        REQUIRE(dataModel.hasExpectedValueRange() == true);
    }

    SECTION("correct value range"){
        dataModel.setUnit(CalibrationDataModel::Unit::gHA_cm3);
        dataModel.appendRow(100, 0.12);
        dataModel.appendRow(120, 0.15);
        REQUIRE(dataModel.hasExpectedValueRange() == true);
    }
}