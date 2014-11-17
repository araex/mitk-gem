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


#ifndef PaddingView_h
#define PaddingView_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_PaddingViewControls.h"

#include "lib/WorkbenchUtils/WorkbenchUtils.h"


/**
  \brief PaddingView

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class PaddingView : public QmitkAbstractView {
  // this is needed for all Qt objects that should have a Qt meta-object
  // (everything that derives from QObject and wants to have signal/slots)
  Q_OBJECT

  public:

    static const std::string VIEW_ID;

  protected slots:
    void padLeftButtonPressed();
    void padRightButtonPressed();
    void padUpButtonPressed();
    void padDownButtonPressed();

  protected:

    virtual void CreateQtPartControl(QWidget *parent);

    virtual void SetFocus();

    /// \brief called by QmitkFunctionality when DataManager's selection has changed
    virtual void OnSelectionChanged( berry::IWorkbenchPart::Pointer source,
                                     const QList<mitk::DataNode::Pointer>& nodes );

    Ui::PaddingViewControls m_Controls;

private:
    void addPadding(WorkbenchUtils::Axis axis, bool append);
    void refreshBoundaries();

};

#endif // PaddingView_h
