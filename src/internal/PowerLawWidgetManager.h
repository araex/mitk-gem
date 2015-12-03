#pragma once

#include <QWidget>
#include <QLayout>

#include "PowerLawWidget.h"
#include "PowerLawFunctor.h"

class PowerLawWidgetManager : public QObject {
    Q_OBJECT

public:
    PowerLawWidgetManager(QWidget *_parent);
    PowerLawFunctor createFunctor();
    PowerLawWidget * getWidget(size_t);

public slots:
    PowerLawWidget* addPowerLaw();
    bool removePowerLaw();

private:
    void updateConnections();

    QWidget *m_Parent;
    std::vector<PowerLawWidget *> m_Widgets;
};