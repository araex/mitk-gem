/**
 *  MITK-GEM: Volume Mesher Plugin
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *  Copyright (c) 2016, ETH Zurich, Institute for Biomechanics, B. Helgason
 *  Copyright (c) 2016, University of Iceland, Mechanical Engineering and Computer Science, H. Pállson
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#include "TetgenOptionRow.h"

#include <QSpacerItem>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QHBoxLayout>

TetgenOptionRow::TetgenOptionRow(std::string _flag, std::string _description, ValueType _type, QGridLayout *_parent)
: m_Flag(_flag)
, m_Description(_description)
, m_ValueType(_type)
{
    m_CheckBox = new QCheckBox();
    m_CheckBox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    m_CheckBox->setMinimumWidth(30);

    switch (getType()){
        case ValueType::NONE:
            m_ValueWidget = new QAbstractSpinBox();
            m_ValueWidget->setHidden(true);
            break;
        case ValueType::INT:{
            auto widget = new QSpinBox();
            widget->setRange(std::numeric_limits<int>::lowest(), std::numeric_limits<int>::max());
            widget->setSingleStep(1);
            m_ValueWidget = widget;
            break;
        }

        case ValueType::FLOAT:{
            auto widget = new QDoubleSpinBox();
            widget->setRange(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
            widget->setSingleStep(1.0);
            widget->setDecimals(2);
            m_ValueWidget = widget;
            break;
        }
    }
    m_ValueWidget->setButtonSymbols( QAbstractSpinBox::NoButtons );
    m_ValueWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);

    auto flag = new QLabel(QObject::tr(_flag.c_str()));

    auto description = new QLabel(QObject::tr(_description.c_str()));
    description->setWordWrap(true);

    auto row = _parent->rowCount();
    _parent->addWidget(m_CheckBox, row, 0);
    _parent->addWidget(flag, row, 1);
    _parent->addWidget(m_ValueWidget, row, 2);
    _parent->addWidget(description, row, 3);
    _parent->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed), row, 4);
}

TetgenOptionRow::ValueType TetgenOptionRow::getType() {
    return m_ValueType;
}

bool TetgenOptionRow::isChecked() {
    return m_CheckBox->isChecked();
}

void TetgenOptionRow::setChecked(bool _b) {
    m_CheckBox->setChecked(_b);
}

bool TetgenOptionRow::hasValue() {
    return getType() == ValueType::INT || getType() == ValueType::FLOAT;
}

int TetgenOptionRow::getIntValue() {
    return static_cast<QSpinBox*>(m_ValueWidget)->value();
}

float TetgenOptionRow::getFloatValue() {
    return static_cast<QDoubleSpinBox*>(m_ValueWidget)->value();
}

void TetgenOptionRow::setIntValue(int _i) {
    static_cast<QSpinBox*>(m_ValueWidget)->setValue(_i);
}

void TetgenOptionRow::setFloatValue(float _f) {
    static_cast<QDoubleSpinBox*>(m_ValueWidget)->setValue(_f);
}