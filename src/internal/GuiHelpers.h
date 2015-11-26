#pragma once

#include <QWidget>

namespace gui {
    void setQStyleSheetField(QWidget *widget, const char *fieldName, bool bEnabled){
        widget->setProperty(fieldName, bEnabled);
        widget->style()->unpolish(widget); // need to do this since we changed the stylesheet
        widget->style()->polish(widget);
        widget->update();
    }

    void setMandatoryField(QWidget *widget, bool bEnabled){
        setQStyleSheetField(widget, "mandatoryField", bEnabled);
    }

    void setWarningField(QWidget *widget, bool bEnabled){
        setQStyleSheetField(widget, "warningField", bEnabled);
    }

    void setErrorField(QWidget *widget, bool bEnabled){
        setQStyleSheetField(widget, "errorField", bEnabled);
    }
}