#pragma once

#include <berryISelectionListener.h>
#include <QmitkAbstractView.h>

#include "ui_VolumeMeshViewControls.h"
#include "TetgenOptionGrid.h"

class VolumeMeshView : public QmitkAbstractView {
    Q_OBJECT

public:
    static const std::string VIEW_ID;

protected slots:
    void generateButtonClicked();

protected:
    virtual void CreateQtPartControl(QWidget *parent) override;
    virtual void SetFocus() override;

    Ui::VolumeMeshViewControls m_Controls;
    TetgenOptionGrid m_TetgenOptionGrid;
};
