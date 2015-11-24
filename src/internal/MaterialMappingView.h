/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#pragma once

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_MaterialMappingViewControls.h"
#include "CalibrationDataModel.h"

class MaterialMappingView : public QmitkAbstractView {
    Q_OBJECT

public:
    static const std::string VIEW_ID;
    MaterialMappingView();

protected slots:
    void deleteSelectedRows();
    void loadButtonClicked();
    void saveButtonClicked();

protected:
    virtual void CreateQtPartControl(QWidget *parent) override;
    virtual void SetFocus() override;
    virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source, const QList <mitk::DataNode::Pointer> &nodes) override;

    Ui::MaterialMappingViewControls m_Controls;
    CalibrationDataModel m_CalibrationDataModel;
};
