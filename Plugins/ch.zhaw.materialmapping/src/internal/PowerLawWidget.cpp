#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>

#include <limits>

#include "PowerLawWidget.h"

const double PowerLawWidget::MinValue = std::numeric_limits<float>::lowest();
const double PowerLawWidget::MaxValue = std::numeric_limits<float>::max();

PowerLawWidget::PowerLawWidget() {
    m_Factor = new QDoubleSpinBox;
    m_Factor->setRange(MinValue, MaxValue);
    m_Factor->setSingleStep(0.01);
    m_Factor->setDecimals(3);
    m_Factor->setValue(1.0);
    m_Factor->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_Factor->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_Factor->setMinimumWidth(75);
    m_Factor->setMaximumWidth(250);

    m_Exponent = new QDoubleSpinBox;
    m_Exponent->setRange(0.001, MaxValue);
    m_Exponent->setSingleStep(0.01);
    m_Exponent->setDecimals(3);
    m_Exponent->setValue(1.0);
    m_Exponent->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_Exponent->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_Exponent->setMinimumWidth(75);
    m_Exponent->setMaximumWidth(200);

    m_Offset = new QDoubleSpinBox;
    m_Offset->setRange(MinValue, MaxValue);
    m_Offset->setSingleStep(1.0);
    m_Offset->setDecimals(3);
    m_Offset->setValue(0.0);
    m_Offset->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_Offset->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_Offset->setMinimumWidth(75);
    m_Offset->setMaximumWidth(250);

    m_Min = new QDoubleSpinBox;
    m_Min->setRange(MinValue, MaxValue);
    m_Min->setSingleStep(1.0);
    m_Min->setDecimals(2);
    m_Min->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_Min->setSpecialValueText(tr("min"));
    m_Min->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_Min->setMinimumWidth(50);
    m_Min->setMaximumWidth(150);
    lockMin(true);

    m_Max = new QDoubleSpinBox;
    m_Max->setRange(MinValue, MaxValue);
    m_Max->setSingleStep(1.0);
    m_Max->setDecimals(2);
    m_Max->setValue(MinValue); // we're using the setSpecialValue functionality to show text, but that requires the value to be equal to minimum()
    m_Max->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_Max->setSpecialValueText(tr("max"));
    m_Max->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_Max->setMinimumWidth(50);
    m_Max->setMaximumWidth(150);
    lockMax(true);

    auto formulaLayout = new QHBoxLayout;
    formulaLayout->setContentsMargins(0, 0, 0, 0);
    formulaLayout->setMargin(0);
    formulaLayout->setSpacing(0);
    formulaLayout->addWidget(new QLabel(tr("E=")));
    formulaLayout->addWidget(m_Factor);
    formulaLayout->addWidget(new QLabel(tr("*ρ^")));
    formulaLayout->addWidget(m_Exponent);
    formulaLayout->addWidget(new QLabel(tr("+")));
    formulaLayout->addWidget(m_Offset);
    formulaLayout->addSpacerItem(new QSpacerItem(15, 1));
    formulaLayout->addWidget(m_Min);
    formulaLayout->addWidget(new QLabel(tr("≤ρ<")));
    formulaLayout->addWidget(m_Max);

    auto formulaWidget = new QWidget;
    formulaWidget->setLayout(formulaLayout);

    auto mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(formulaWidget);
    setLayout(mainLayout);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

void PowerLawWidget::lockMin(bool _b) {
    m_Min->setEnabled(!_b);
    m_Min->setReadOnly(_b);
    m_Min->setValue(MinValue);
}

void PowerLawWidget::lockMax(bool _b) {
    m_Max->setEnabled(!_b);
    m_Max->setReadOnly(_b);
    m_Max->setValue(MinValue); // we're using the setSpecialValue functionality to show text, but that requires the value to be equal to minimum()
}

void PowerLawWidget::setMin(double _d) {
    lockMin(false);
    m_Min->setValue(_d);
}

void PowerLawWidget::setMax(double _d) {
    lockMax(false);
    m_Max->setValue(_d);
}

double PowerLawWidget::getMin() {
    return m_Min->value();
}

double PowerLawWidget::getMax() {
    return m_Max->value();
}

PowerLawParameters PowerLawWidget::getPowerLawParameters() {
    return PowerLawParameters(m_Factor->value(), m_Exponent->value(), m_Offset->value());
}

void PowerLawWidget::connect(PowerLawWidget *_other) {
    QWidget::connect(m_Max, SIGNAL(valueChanged(double)), _other->m_Min, SLOT(setValue(double)));
    QWidget::connect(_other->m_Min, SIGNAL(valueChanged(double)), m_Max, SLOT(setValue(double)));
}

void PowerLawWidget::setFactor(double _d) {
    m_Factor->setValue(_d);
}

void PowerLawWidget::setExponent(double _d) {
    m_Exponent->setValue(_d);
}

void PowerLawWidget::setOffset(double _d) {
    m_Offset->setValue(_d);
}
