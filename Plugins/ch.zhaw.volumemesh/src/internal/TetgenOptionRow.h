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

#pragma once

#include <functional>
#include <QObject>
#include <QWidget>
#include <QCheckBox>
#include <QAbstractSpinBox>
#include <QGridLayout>

class TetgenOptionRow {
public:
    enum class ValueType {
        NONE, INT, FLOAT
    };

    TetgenOptionRow(std::string _flag, std::string _description, ValueType, QGridLayout*);
    ValueType getType();
    bool isChecked();
    void setChecked(bool);

    bool hasValue();
    int getIntValue();
    float getFloatValue();
    void setIntValue(int);
    void setFloatValue(float);

private:
    std::string m_Flag, m_Description;
    ValueType m_ValueType;

    QCheckBox *m_CheckBox;
    QAbstractSpinBox *m_ValueWidget;
};