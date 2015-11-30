#pragma once

#include <QWidget>
#include <QLayout>

#include "PowerLawWidget.h"

class PowerLawWidgetManager {
public:
    PowerLawWidgetManager(QWidget *_parent);

    void addPowerLaw();

private:
    void updateConnections();

    QWidget *m_Parent;
    std::vector<PowerLawWidget *> m_Widgets;
};