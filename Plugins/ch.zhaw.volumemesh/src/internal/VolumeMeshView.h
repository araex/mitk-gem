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
