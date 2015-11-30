#pragma once

#include <QWidget>
#include <QDoubleSpinBox>

#include "PowerLawFunctor.h"

class PowerLawWidget : public QWidget {
    Q_OBJECT

public:
    PowerLawWidget();
    PowerLawParameters getPowerLawParameters();

private:
    QDoubleSpinBox *m_Factor, *m_Exponent, *m_Offset, *m_Min, *m_Max;
    double m_MinVal, m_MaxVal;
};