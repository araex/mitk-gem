#pragma once

#include <cmath>

#include <QWidget>
#include <QDoubleSpinBox>

#include "PowerLawFunctor.h"

class PowerLawWidget : public QWidget {
    Q_OBJECT

public:
    static constexpr double MinValue = -9999, MaxValue = 9999;
    static double Range() {
        return std::abs(MinValue) + std::abs(MaxValue);
    }
    PowerLawWidget();
    PowerLawParameters getPowerLawParameters();

    void lockMin(bool _b);
    void lockMax(bool _b);
    void setMin(double _d);
    void setMax(double _d);
    double getMin();
    double getMax();

    void connect(PowerLawWidget *_other);

protected:
    QDoubleSpinBox *m_Factor, *m_Exponent, *m_Offset, *m_Min, *m_Max;
};