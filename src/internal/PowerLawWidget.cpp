#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>

#include <limits>

#include "PowerLawWidget.h"

PowerLawWidget::PowerLawWidget(){
    m_Factor = new QDoubleSpinBox;
    m_Factor->setRange(-99.0, 99.0);
    m_Factor->setSingleStep(0.01);
    m_Factor->setDecimals(3);
    m_Factor->setValue(1.0);
    m_Factor->setButtonSymbols( QAbstractSpinBox::NoButtons );

    m_Exponent = new QDoubleSpinBox;
    m_Exponent->setRange(-99.0, 99.0);
    m_Exponent->setSingleStep(0.01);
    m_Exponent->setDecimals(3);
    m_Exponent->setValue(1.0);
    m_Exponent->setButtonSymbols( QAbstractSpinBox::NoButtons );

    m_Offset = new QDoubleSpinBox;
    m_Offset->setRange(-9999.0, 9999.0);
    m_Offset->setSingleStep(1.0);
    m_Offset->setDecimals(3);
    m_Offset->setValue(0.0);
    m_Offset->setButtonSymbols( QAbstractSpinBox::NoButtons );

    m_Min = new QDoubleSpinBox;
    m_Min->setRange(MinValue, MaxValue);
    m_Min->setSingleStep(1.0);
    m_Min->setDecimals(2);
    m_Min->setButtonSymbols( QAbstractSpinBox::NoButtons );
    m_Min->setSpecialValueText(tr("min"));
    lockMin(true);

    m_Max = new QDoubleSpinBox;
    m_Max->setRange(MinValue, MaxValue);
    m_Max->setSingleStep(1.0);
    m_Max->setDecimals(2);
    m_Max->setValue(MinValue); // we're using the setSpecialValue functionality to show text, but that requires the value to be equal to minimum()
    m_Max->setButtonSymbols( QAbstractSpinBox::NoButtons );
    m_Max->setSpecialValueText(tr("max"));
    lockMax(true);

    auto formulaLayout = new QHBoxLayout;
    formulaLayout->setContentsMargins(0,0,0,0);
    formulaLayout->setMargin(0);
    formulaLayout->setSpacing(0);
    formulaLayout->addWidget(new QLabel(tr("E = ")));
    formulaLayout->addWidget(m_Factor);
    formulaLayout->addWidget(new QLabel(tr("* ρ ^")));
    formulaLayout->addWidget(m_Exponent);
    formulaLayout->addWidget(new QLabel(tr("+")));
    formulaLayout->addWidget(m_Offset);
    formulaLayout->addStretch();
    formulaLayout->addWidget(m_Min);
    formulaLayout->addWidget(new QLabel(tr(" ≤ ρ < ")));
    formulaLayout->addWidget(m_Max);

    auto formulaWidget = new QWidget;
    formulaWidget->setLayout(formulaLayout);

    auto mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(formulaWidget);
    setLayout(mainLayout);
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