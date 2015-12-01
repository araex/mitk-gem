#pragma once

#include <QWidget>
#include <QLayout>

#include "PowerLawWidget.h"
#include "PowerLawFunctor.h"

class PowerLawWidgetManager {
public:
    PowerLawWidgetManager(QWidget *_parent);

    PowerLawWidget* addPowerLaw();
    PowerLawFunctor createFunctor();

private:
    void updateConnections();

    QWidget *m_Parent;
    std::vector<PowerLawWidget *> m_Widgets;
};