#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>

#include <limits>

#include "PowerLawWidget.h"

PowerLawWidget::PowerLawWidget() {
    m_MinVal = -9999;
    m_MaxVal = 9999;
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
    m_Min->setRange(m_MinVal, m_MaxVal);
    m_Min->setSingleStep(1.0);
    m_Min->setDecimals(2);
    m_Min->setValue(std::numeric_limits<float>::lowest());
    m_Min->setButtonSymbols( QAbstractSpinBox::NoButtons );
    m_Min->setSpecialValueText(tr("min"));
    m_Min->setEnabled(false);
    m_Min->setReadOnly(true);

    m_Max = new QDoubleSpinBox;
    m_Max->setRange(m_MinVal, m_MaxVal);
    m_Max->setSingleStep(1.0);
    m_Max->setDecimals(2);
    m_Max->setValue(m_MinVal); // we're using the setSpecialValue functionality to show text, but that requires the value to be equal to minimum()
    m_Max->setButtonSymbols( QAbstractSpinBox::NoButtons );
    m_Max->setSpecialValueText(tr("max"));
    m_Max->setEnabled(false);
    m_Max->setReadOnly(true);

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
    formulaLayout->addWidget(new QLabel(tr(" ≤ ρ ≤ ")));
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

PowerLawParameters PowerLawWidget::getPowerLawParameters() {
    return PowerLawParameters(m_Factor->value(), m_Exponent->value(), m_Offset->value());
}