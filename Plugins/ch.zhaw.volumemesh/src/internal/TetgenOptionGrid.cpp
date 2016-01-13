#include "TetgenOptionGrid.h"

TetgenOptionGrid::TetgenOptionGrid() {
    m_GridLayout = new QGridLayout;
    m_GridLayout->setContentsMargins(0,0,0,0);
    m_GridLayout->setMargin(0);
    m_GridLayout->setSpacing(-1);
    setLayout(m_GridLayout);
}

void TetgenOptionGrid::addOption(std::string _flag, std::string _description, int tetgenbehavior::* _option) {
    auto widget = new TetgenOptionRow(_flag, _description, TetgenOptionRow::ValueType::NONE, m_GridLayout);
    auto checked = m_DefaultOptions.*_option;
    widget->setChecked(checked);

    auto pair = std::make_tuple(widget, _option);
    m_NoneOptions.push_back(pair);
}

void TetgenOptionGrid::addIntOption(std::string _flag, std::string _description, int tetgenbehavior::* _option, int tetgenbehavior::* _value) {
    auto widget = new TetgenOptionRow(_flag, _description, TetgenOptionRow::ValueType::INT, m_GridLayout);
    auto checked = m_DefaultOptions.*_option;
    auto value = m_DefaultOptions.*_value;
    widget->setChecked(checked);
    widget->setIntValue(value);

    auto triplet = std::make_tuple(widget, _option, _value);
    m_IntOptions.push_back(triplet);
}

void TetgenOptionGrid::addFloatOption(std::string _flag, std::string _description, int tetgenbehavior::* _option, REAL tetgenbehavior::* _value) {
    auto widget = new TetgenOptionRow(_flag, _description, TetgenOptionRow::ValueType::FLOAT, m_GridLayout);
    auto checked = m_DefaultOptions.*_option;
    auto value = m_DefaultOptions.*_value;
    widget->setChecked(checked);
    widget->setFloatValue(value);

    auto triplet = std::make_tuple(widget, _option, _value);
    m_FloatOptions.push_back(triplet);
}

tetgenbehavior TetgenOptionGrid::getOptionsFromGui() {
    tetgenbehavior options = m_DefaultOptions;

    TetgenOptionRow *row;
    int tetgenbehavior::* option;
    int tetgenbehavior::* intValue;
    REAL tetgenbehavior::* floatValue;

    for(const auto &t : m_NoneOptions){
        std::tie(row, option) = t;
        options.*option = row->isChecked();
    }

    for(const auto &t : m_IntOptions){
        std::tie(row, option, intValue) = t;
        options.*option = row->isChecked();
        options.*intValue = row->getIntValue();
    }

    for(const auto &t : m_FloatOptions){
        std::tie(row, option, floatValue) = t;
        options.*option = row->isChecked();
        options.*floatValue = row->getFloatValue();
    }

    return options;
}