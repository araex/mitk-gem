#pragma once

#include <cmath>
#include <limits>
#include <QWidget>
#include <QDoubleSpinBox>

#include "PowerLawFunctor.h"

class PowerLawWidget : public QWidget {
    Q_OBJECT

public:
    static const double MinValue, MaxValue;

    PowerLawWidget();
    PowerLawParameters getPowerLawParameters();

    void lockMin(bool _b);
    void lockMax(bool _b);
    void setMin(double _d);
    void setMax(double _d);
    double getMin();
    double getMax();

    void connect(PowerLawWidget *_other);

    QDoubleSpinBox *m_Factor, *m_Exponent, *m_Offset, *m_Min, *m_Max;
};