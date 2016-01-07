#pragma once

#include <QWidget>
#include <tinyxml.h>

#include "ui_MaterialMappingViewControls.h"

#include "BoneDensityFunctor.h"
#include "CalibrationDataModel.h"

namespace gui {
    BoneDensityFunctor createDensityFunctorFromGui(Ui::MaterialMappingViewControls &_controls, CalibrationDataModel &_dataModel);

    // XML import / export
    TiXmlElement* serializeDensityParametersToXml(Ui::MaterialMappingViewControls &_controls);
    void loadDensityParametersFromXml(Ui::MaterialMappingViewControls &_controls, TiXmlElement *);
    TiXmlElement* serializeOptionsToXml(Ui::MaterialMappingViewControls &_controls);
    void loadOptionsFromXml(Ui::MaterialMappingViewControls &_controls, TiXmlElement *);

    void setQStyleSheetField(QWidget *widget, const char *fieldName, bool bEnabled);
    void setMandatoryField(QWidget *widget, bool bEnabled);
    void setWarningField(QWidget *widget, bool bEnabled);
    void setErrorField(QWidget *widget, bool bEnabled);
}