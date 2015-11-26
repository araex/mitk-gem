#pragma once

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_MaterialMappingViewControls.h"
#include "CalibrationDataModel.h"

class MaterialMappingView : public QmitkAbstractView {
    Q_OBJECT

public:
    static const std::string VIEW_ID;

protected slots:
    void deleteSelectedRows();
    void loadButtonClicked();
    void saveButtonClicked();
    void startButtonClicked();

protected:
    virtual void CreateQtPartControl(QWidget *parent) override;
    virtual void SetFocus() override {}; // required by blueberry
    bool isValidSelection();

    Ui::MaterialMappingViewControls m_Controls;
    CalibrationDataModel m_CalibrationDataModel;
};
