#pragma once

#include <QWidget>
#include <QGridLayout>

#include "TetgenOptionRow.h"

class TetgenOptionGrid : public QWidget {
    Q_OBJECT

public:
    TetgenOptionGrid();
    void addOption(std::string _flag, std::string _description);
    void addIntOption(std::string _flag, std::string _description);
    void addFloatOption(std::string _flag, std::string _description);

private:
    QGridLayout *m_GridLayout;
};