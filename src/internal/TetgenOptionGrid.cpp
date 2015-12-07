#include "TetgenOptionGrid.h"

TetgenOptionGrid::TetgenOptionGrid() {
    m_GridLayout = new QGridLayout;
    m_GridLayout->setContentsMargins(0,0,0,0);
    m_GridLayout->setMargin(0);
    m_GridLayout->setSpacing(-1);
    setLayout(m_GridLayout);
}

void TetgenOptionGrid::addOption(std::string _flag, std::string _description) {
    auto widget = new TetgenOptionRow(_flag, _description, TetgenOptionRow::ValueType::NONE, m_GridLayout);
}

void TetgenOptionGrid::addIntOption(std::string _flag, std::string _description) {
    auto widget = new TetgenOptionRow(_flag, _description, TetgenOptionRow::ValueType::INT, m_GridLayout);
}

void TetgenOptionGrid::addFloatOption(std::string _flag, std::string _description) {
    auto widget = new TetgenOptionRow(_flag, _description, TetgenOptionRow::ValueType::FLOAT, m_GridLayout);
}