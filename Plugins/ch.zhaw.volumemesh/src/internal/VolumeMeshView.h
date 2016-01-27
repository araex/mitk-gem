#pragma once

#include <berryISelectionListener.h>
#include <QmitkAbstractView.h>
#include <QFuture>

#include "ui_VolumeMeshViewControls.h"
#include "TetgenOptionGrid.h"

class VolumeMeshView : public QmitkAbstractView {
    Q_OBJECT

public:
    ~VolumeMeshView();

    static const std::string VIEW_ID;

    signals:
    void invalidMeshingResultDetected();

protected slots:
    void generateButtonClicked();
    void meshingFailed();

protected:
    virtual void CreateQtPartControl(QWidget *parent) override;
    virtual void SetFocus() override;

    Ui::VolumeMeshViewControls m_Controls;
    TetgenOptionGrid m_TetgenOptionGrid;

    QFuture<void> m_WorkerFuture;
};
