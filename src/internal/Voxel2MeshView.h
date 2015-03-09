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


#ifndef Voxel2MeshView_h
#define Voxel2MeshView_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_Voxel2MeshViewControls.h"

class Voxel2MeshView : public QmitkAbstractView {
    Q_OBJECT

public:

    static const std::string VIEW_ID;

protected slots:

protected:
    virtual void CreateQtPartControl(QWidget *parent);

    virtual void SetFocus();
    virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source, const QList <mitk::DataNode::Pointer> &nodes);

    Ui::Voxel2MeshViewControls m_Controls;

};

#endif // Voxel2MeshView_h
