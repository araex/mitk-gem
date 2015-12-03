#pragma once

#include <berryISelectionListener.h>
#include <QmitkAbstractView.h>

#include "ui_VolumeMeshViewControls.h"

class VolumeMeshView : public QmitkAbstractView{
  Q_OBJECT

  public:
    static const std::string VIEW_ID;

  protected slots:
  protected:
    virtual void CreateQtPartControl(QWidget *parent) override;
    virtual void SetFocus() override;
    Ui::VolumeMeshViewControls m_Controls;
};
