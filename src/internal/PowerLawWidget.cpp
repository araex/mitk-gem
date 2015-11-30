#include <QLabel>
#include <QDoubleSpinBox>
#include <QHBoxLayout>

#include "PowerLawWidget.h"

PowerLawWidget::PowerLawWidget() {
    auto factorSpinBox = new QDoubleSpinBox;
    factorSpinBox->setRange(-99.0, 99.0);
    factorSpinBox->setSingleStep(0.01);
    factorSpinBox->setDecimals(3);
    factorSpinBox->setValue(1.0);

    auto exponentSpinBox = new QDoubleSpinBox;
    exponentSpinBox->setRange(-99.0, 99.0);
    exponentSpinBox->setSingleStep(0.01);
    exponentSpinBox->setDecimals(3);
    exponentSpinBox->setValue(1.0);

    auto offsetSpinBox = new QDoubleSpinBox;
    offsetSpinBox->setRange(-9999.0, 9999.0);
    offsetSpinBox->setSingleStep(1.0);
    offsetSpinBox->setDecimals(3);
    offsetSpinBox->setValue(0.0);

    auto formulaLayout = new QHBoxLayout;
    formulaLayout->setContentsMargins(0,0,0,0);
    formulaLayout->setMargin(0);
    formulaLayout->setSpacing(0);
    formulaLayout->addWidget(new QLabel(tr("E = ")));
    formulaLayout->addWidget(factorSpinBox);
    formulaLayout->addWidget(new QLabel(tr("* ρ ^")));
    formulaLayout->addWidget(exponentSpinBox);
    formulaLayout->addWidget(new QLabel(tr("+")));
    formulaLayout->addWidget(offsetSpinBox);
    formulaLayout->addStretch();
    formulaLayout->addWidget(new QLabel(tr("min")));
    formulaLayout->addWidget(new QLabel(tr(" ≤ ρ ≤ ")));
    formulaLayout->addWidget(new QLabel(tr("max")));

    auto formulaWidget = new QWidget;
    formulaWidget->setLayout(formulaLayout);

    auto mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(formulaWidget);
    setLayout(mainLayout);
}