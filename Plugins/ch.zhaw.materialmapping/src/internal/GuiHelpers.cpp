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

TiXmlElement * gui::serializeDensityParametersToXml(Ui::MaterialMappingViewControls &_controls) {
    auto root = new TiXmlElement("BoneDensityParameters");

    auto rhoCt = new TiXmlElement("RhoCT");
    rhoCt->SetAttribute("AutomaticFit", _controls.automaticFitCheckBox->isChecked());
    rhoCt->SetAttribute("slope", _controls.linEQSlopeSpinBox->value());
    rhoCt->SetAttribute("offset", _controls.linEQOffsetSpinBox->value());

    auto rhoAsh = new TiXmlElement("RhoAsh");
    rhoAsh->SetAttribute("enabled", _controls.rhoAshCheckBox->isChecked());
    rhoAsh->SetAttribute("offset", _controls.rhoAshOffsetSpinBox->value());
    rhoAsh->SetAttribute("divisor", _controls.rhoAshDivisorSpinBox->value());

    auto rhoApp = new TiXmlElement("RhoApp");
    rhoApp->SetAttribute("enabled", _controls.rhoAppCheckBox->isChecked());
    rhoApp->SetAttribute("divisor", _controls.rhoAppDivisorSpinBox->value());

    root->LinkEndChild(rhoCt);
    root->LinkEndChild(rhoAsh);
    root->LinkEndChild(rhoApp);

    return root;
}

void gui::loadDensityParametersFromXml(Ui::MaterialMappingViewControls &_controls, TiXmlElement* _root) {
    bool b;
    double d;
    int ret;

    auto rhoCt = _root->FirstChildElement("RhoCT");
    ret = rhoCt->QueryBoolAttribute("AutomaticFit", &b);
    if(ret == TIXML_SUCCESS){
        _controls.automaticFitCheckBox->setChecked(b);
    }
    ret = rhoCt->QueryDoubleAttribute("slope", &d);
    if(ret == TIXML_SUCCESS){
        _controls.linEQSlopeSpinBox->setValue(d);
    }
    ret = rhoCt->QueryDoubleAttribute("offset", &d);
    if(ret == TIXML_SUCCESS){
        _controls.linEQOffsetSpinBox->setValue(d);
    }

    auto rhoAsh = _root->FirstChildElement("RhoAsh");
    ret = rhoAsh->QueryBoolAttribute("enabled", &b);
    if(ret == TIXML_SUCCESS){
        _controls.rhoAshCheckBox->setChecked(b);
    }
    ret = rhoAsh->QueryDoubleAttribute("offset", &d);
    if(ret == TIXML_SUCCESS){
        _controls.rhoAshOffsetSpinBox->setValue(d);
    }
    ret = rhoAsh->QueryDoubleAttribute("divisor", &d);
    if(ret == TIXML_SUCCESS){
        _controls.rhoAshDivisorSpinBox->setValue(d);
    }

    auto rhoApp = _root->FirstChildElement("RhoApp");
    ret = rhoApp->QueryBoolAttribute("enabled", &b);
    if(ret == TIXML_SUCCESS){
        _controls.rhoAppCheckBox->setChecked(b);
    }
    ret = rhoApp->QueryDoubleAttribute("divisor", &d);
    if(ret == TIXML_SUCCESS){
        _controls.rhoAppDivisorSpinBox->setValue(d);
    }
}

TiXmlElement * gui::serializeOptionsToXml(Ui::MaterialMappingViewControls &_controls) {
    auto root = new TiXmlElement("Options");
    root->SetAttribute("doPeel", _controls.uParamCheckBox->isChecked());
    root->SetAttribute("numberOfExtends", _controls.eParamSpinBox->value());
    root->SetAttribute("minValue", _controls.fParamSpinBox->value());

    return root;
}

void gui::loadOptionsFromXml(Ui::MaterialMappingViewControls &_controls, TiXmlElement *_root) {
    bool b;
    int i;
    double d;

    auto ret = _root->QueryBoolAttribute("doPeel", &b);
    if(ret == TIXML_SUCCESS){
        _controls.uParamCheckBox->setChecked(b);
    }

    ret = _root->QueryIntAttribute("numberOfExtends", &i);
    if(ret == TIXML_SUCCESS){
        _controls.eParamSpinBox->setValue(i);
    }

    ret = _root->QueryDoubleAttribute("minValue", &d);
    if(ret == TIXML_SUCCESS){
        _controls.fParamSpinBox->setValue(d);
    }
}