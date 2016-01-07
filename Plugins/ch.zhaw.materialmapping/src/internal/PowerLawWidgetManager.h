#pragma once

#include <QWidget>
#include <QLayout>

#include <tinyxml.h>

#include "PowerLawWidget.h"
#include "PowerLawFunctor.h"

class PowerLawWidgetManager : public QObject {
    Q_OBJECT

public:
    PowerLawWidgetManager(QWidget *_parent);
    PowerLawFunctor createFunctor();
    PowerLawWidget * getWidget(size_t);
    size_t getNumberOfWidgets();

    TiXmlElement* serializeToXml();
    void loadFromXml(TiXmlElement*);

public slots:
    PowerLawWidget* addPowerLaw();
    bool removePowerLaw();

private:
    void setPowerLawWidgets(std::vector<PowerLawWidget*>);
    void updateConnections();

    QWidget *m_Parent;
    std::vector<PowerLawWidget *> m_Widgets;
};