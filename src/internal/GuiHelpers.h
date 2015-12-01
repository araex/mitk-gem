#pragma once

#include <QWidget>

#include "ui_MaterialMappingViewControls.h"

#include "BoneDensityFunctor.h"
#include "CalibrationDataModel.h"

namespace gui {
    BoneDensityFunctor createDensityFunctorFromGui(Ui::MaterialMappingViewControls &_controls, CalibrationDataModel &_dataModel);

    void setQStyleSheetField(QWidget *widget, const char *fieldName, bool bEnabled);
    void setMandatoryField(QWidget *widget, bool bEnabled);
    void setWarningField(QWidget *widget, bool bEnabled);
    void setErrorField(QWidget *widget, bool bEnabled);
}