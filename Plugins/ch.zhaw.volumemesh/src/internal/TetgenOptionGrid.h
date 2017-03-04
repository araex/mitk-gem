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

#include "tetgen.h"
#include "TetgenOptionRow.h"
#include <QGridLayout>
#include <QWidget>

class TetgenOptionGrid : public QWidget {
    Q_OBJECT

public:
    TetgenOptionGrid();
    void setDefaultOptions(tetgenbehavior _o){
        m_DefaultOptions = _o;
    }

    void addOption(std::string _flag, std::string _description, int tetgenbehavior::* _option);
    void addIntOption(std::string _flag, std::string _description, int tetgenbehavior::* _option, int tetgenbehavior::* _value);
    void addFloatOption(std::string _flag, std::string _description, int tetgenbehavior::* _option, REAL tetgenbehavior::* _value);

    tetgenbehavior getOptionsFromGui();

private:
    QGridLayout *m_GridLayout;
    tetgenbehavior m_DefaultOptions;

    std::vector<std::tuple<TetgenOptionRow*, int tetgenbehavior::*>> m_NoneOptions;
    std::vector<std::tuple<TetgenOptionRow*, int tetgenbehavior::*, int tetgenbehavior::*>> m_IntOptions;
    std::vector<std::tuple<TetgenOptionRow*, int tetgenbehavior::*, REAL tetgenbehavior::*>> m_FloatOptions;
};