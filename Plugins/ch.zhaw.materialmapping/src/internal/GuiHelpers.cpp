#include "GuiHelpers.h"

BoneDensityFunctor gui::createDensityFunctorFromGui(Ui::MaterialMappingViewControls &_controls, CalibrationDataModel &_dataModel) {
    BoneDensityFunctor ret;
    ret.SetRhoCt(_dataModel.getFittedLine());

    if(_controls.rhoAshCheckBox->isChecked()){
        auto rhoAsh_offset = _controls.rhoAshOffsetSpinBox->value();
        auto rhoAsh_divisor = _controls.rhoAshDivisorSpinBox->value();
        BoneDensityParameters::RhoAsh rhoAsh(rhoAsh_offset, rhoAsh_divisor);
        ret.SetRhoAsh(rhoAsh);

        if(_controls.rhoAppCheckBox->isChecked()){
            auto rhoApp_divisor = _controls.rhoAppDivisorSpinBox->value();
            BoneDensityParameters::RhoApp rhoApp(rhoApp_divisor);
            ret.SetRhoApp(rhoApp);
        }
    }
    return ret;
}

void gui::setQStyleSheetField(QWidget *widget, const char *fieldName, bool bEnabled){
    widget->setProperty(fieldName, bEnabled);
    widget->style()->unpolish(widget); // need to do this since we changed the stylesheet
    widget->style()->polish(widget);
    widget->update();
}

void gui::setMandatoryField(QWidget *widget, bool bEnabled){
    setQStyleSheetField(widget, "mandatoryField", bEnabled);
}

void gui::setWarningField(QWidget *widget, bool bEnabled){
    setQStyleSheetField(widget, "warningField", bEnabled);
}

void gui::setErrorField(QWidget *widget, bool bEnabled){
    setQStyleSheetField(widget, "errorField", bEnabled);
}