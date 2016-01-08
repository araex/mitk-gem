#pragma once

#include <QWidget>
#include <tinyxml.h>

#include "ui_MaterialMappingViewControls.h"

#include "BoneDensityFunctor.h"
#include "CalibrationDataModel.h"

/**
 * Utility functions to work with the GUI fields
 */
namespace gui {
    // TODO: naming of the functions in here is terrible, refactor

    /**
     * Gathers all field data needed for the rho calculation and creates a functor with them
     */
    BoneDensityFunctor createDensityFunctorFromGui(Ui::MaterialMappingViewControls &_controls, CalibrationDataModel &_dataModel);

    /**
     * Returns a XML representation of the current bone density GUI group state
     */
    TiXmlElement* serializeDensityParametersToXml(Ui::MaterialMappingViewControls &_controls);

    /**
     * Load a bone density GUI group GUI state from XML
     */
    void loadDensityParametersFromXml(Ui::MaterialMappingViewControls &_controls, TiXmlElement *);

    /**
     * Returns a XML representation of the options GUI group state
     */
    TiXmlElement* serializeOptionsToXml(Ui::MaterialMappingViewControls &_controls);

    /**
     * Load a options GUI group state from XML
     */
    void loadOptionsFromXml(Ui::MaterialMappingViewControls &_controls, TiXmlElement *);

    void setQStyleSheetField(QWidget *widget, const char *fieldName, bool bEnabled);
    void setMandatoryField(QWidget *widget, bool bEnabled);
    void setWarningField(QWidget *widget, bool bEnabled);
    void setErrorField(QWidget *widget, bool bEnabled);
}