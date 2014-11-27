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


#ifndef ResampleView_h
#define ResampleView_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>
#include <mitkImage.h>

#include "ui_ResampleViewControls.h"

class ResampleView : public QmitkAbstractView {
  Q_OBJECT

  public:
    static const std::string VIEW_ID;

  protected slots:
    void resampleButtonPressed();
    void copyButtonPressed();

  protected:
    virtual void CreateQtPartControl(QWidget *parent);
    virtual void SetFocus();
    virtual void OnSelectionChanged( berry::IWorkbenchPart::Pointer source, const QList<mitk::DataNode::Pointer>& nodes );

    void updateDimensions(mitk::Image::Pointer);
    void setOriginDimension(unsigned int, unsigned int, unsigned int);
    void setResampleDimension(unsigned int, unsigned int, unsigned int);

    Ui::ResampleViewControls m_Controls;

};

#endif // ResampleView_h
