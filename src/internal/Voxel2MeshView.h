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
#include <mitkImage.h>
#include <mitkSurface.h>

#include "ui_Voxel2MeshViewControls.h"

class Voxel2MeshView : public QmitkAbstractView {
    Q_OBJECT

public:
    struct SurfaceGeneratorParameters{
        bool doMedian;
        int kernelX;
        int kernelY;
        int kernelZ;

        bool doGaussian;
        float deviation;
        float radius;

        int threshold;

        bool doSmoothing;
        int iterations;
        float relaxation;

        bool doDecimation;
        float reduction;
    };

    static const std::string VIEW_ID;

protected slots:
    void generateSurfaceButtonPressed();

protected:
    virtual void CreateQtPartControl(QWidget *parent);

    virtual void SetFocus();
    virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source, const QList <mitk::DataNode::Pointer> &nodes);

    SurfaceGeneratorParameters getParameters();
    mitk::Surface::Pointer createSurface(mitk::Image::Pointer, SurfaceGeneratorParameters params);
    void setMandatoryField(QWidget *widget, bool bEnabled);
    void setQStyleSheetField(QWidget *, const char *, bool);

    Ui::Voxel2MeshViewControls m_Controls;

};

#endif // Voxel2MeshView_h
