#pragma once

#include <QWidget>
#include <QGridLayout>

#include "lib/tetgen1.5.0/tetgen.h"
#include "TetgenOptionRow.h"

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