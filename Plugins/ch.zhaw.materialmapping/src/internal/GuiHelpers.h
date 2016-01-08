#pragma once

#include <QWidget>
#include <tinyxml.h>

#include "ui_MaterialMappingViewControls.h"

#include "BoneDensityFunctor.h"
#include "CalibrationDataModel.h"

/**
 * Utility functions to work with the gui components that are not self managed.
 *
 *      --- CalibrationDataModel  ---
 *     |                             |
 *     |        self managed         |
 *     |                             |
 *      ---  Bone density group   ---
 *     |                             |
 *     |   *managed via GuiHelper*   |
 *     |                             |
 *      --- PowerLawWidgetManager ---
 *     |                             |
 *     |        self managed         |
 *     |                             |
 *      ---     Options group     ---
 *     |                             |
 *     |   *managed via GuiHelper*   |
 *     |                             |
 *      -----------------------------
 *     | save params |  load params  |
 *      -----------------------------
 */
namespace gui {
    /**
     * Gathers all field data needed for the rho calculation and creates a functor with them
     */
    BoneDensityFunctor createDensityFunctor(Ui::MaterialMappingViewControls &_controls, CalibrationDataModel &_dataModel);

    // XML (de-)serialization
    TiXmlElement* serializeDensityGroupStateToXml(Ui::MaterialMappingViewControls &_controls);
    void loadDensityGroupStateFromXml(Ui::MaterialMappingViewControls &_controls, TiXmlElement *);
    TiXmlElement* serializeOptionsGroupStateToXml(Ui::MaterialMappingViewControls &_controls);
    void loadOptionsGroupStateFromXml(Ui::MaterialMappingViewControls &_controls, TiXmlElement *);

    /**
     * Convenience functions to highlight QWidgets with predefined background-colors
     */
    void setMandatoryQSSField(QWidget *widget, bool bEnabled);
    void setWarningQSSField(QWidget *widget, bool bEnabled);
    void setErrorQSSField(QWidget *widget, bool bEnabled);
    void setNamedQSSField(QWidget *widget, const char *fieldName, bool bEnabled);
}