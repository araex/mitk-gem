#pragma once

#include <cmath>
#include <limits>
#include <QWidget>
#include <QDoubleSpinBox>

#include "PowerLawFunctor.h"

/**
 * QWidget for entering power laws and definition ranges of them.
 */
class PowerLawWidget : public QWidget {
    Q_OBJECT

public:
    static const double MinValue, MaxValue;

    PowerLawWidget();
    /**
     * returns the current widget state as a PowerLawParameter
     */
    PowerLawParameters getPowerLawParameters();

    /**
     * Sets whether or not the user can change the lower bound of the definition range
     */
    void lockMin(bool _b);

    /**
     * Sets whether or not the user can change the upper bound of the definition range
     */
    void lockMax(bool _b);

    void setMin(double _d);
    void setMax(double _d);
    double getMin();
    double getMax();

    /**
     * Connects this widgets "max" to another widgets "min" in order to share a definition range boundary. Connected
     * widgets will make sure their shared boundary is always in sync.
     */
    void connect(PowerLawWidget *_other);

    /**
     * TODO: these should be private
     */
    QDoubleSpinBox *m_Factor, *m_Exponent, *m_Offset, *m_Min, *m_Max;
};