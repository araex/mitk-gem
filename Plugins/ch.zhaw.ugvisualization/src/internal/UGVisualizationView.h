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

#include <QmitkFunctionality.h>
#include <mitkUnstructuredGrid.h>
#include <vtkFieldData.h>

#include "ui_QmitkUGVisualizationViewControls.h"

class QWidgetAction;

class QmitkBoolPropertyWidget;

namespace mitk {
    class PropertyObserver;
}

class UGVisualizationView : public QmitkFunctionality {
    Q_OBJECT

public:
    static const std::string VIEW_ID;

    UGVisualizationView();
    virtual ~UGVisualizationView();

    virtual void CreateQtPartControl(QWidget *parent) override;

protected slots:
    void UpdateRenderWindow();
    void RenderingCheckboxClicked(bool clicked);
    void ScalarModeSelectionChanged(int);
    void FieldDataSelectionChanged(int);

protected:
    virtual void OnSelectionChanged(std::vector<mitk::DataNode *> nodes) override;
    void CreateConnections();

private:
    friend class UGVisVolumeObserver;
    void UpdateGUI();
    void ResetGUI();
    void SelectUG(mitk::UnstructuredGrid::Pointer, mitk::DataNode::Pointer);
    void SetFieldDataComboBoxEntries(vtkFieldData *);
    void UpdateFieldDataComboBoxes(mitk::UnstructuredGrid::Pointer);
    void ActivateFieldData(mitk::DataNode::Pointer, QString);
    bool IsRenderable(mitk::DataNode::Pointer);

    Ui::QmitkUGVisualizationViewControls m_Controls;
    QmitkStdMultiWidget *m_MultiWidget;

    int m_FirstVolumeRepId;
    QHash<int, int> m_MapRepComboToEnumId;
    bool m_VolumeMode;
    mitk::PropertyObserver *m_VolumeModeObserver;
    mitk::DataNode::Pointer m_SelectedNode;
};

