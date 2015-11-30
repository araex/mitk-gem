#pragma once

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_MaterialMappingViewControls.h"
#include "CalibrationDataModel.h"
#include "test/Runner.h"
#include "BoneDensityFunctor.h"

class MaterialMappingView : public QmitkAbstractView {
    Q_OBJECT

public:
    static const std::string VIEW_ID;
    static const bool TESTING = true;

protected slots:
    void deleteSelectedRows();
    void loadButtonClicked();
    void saveButtonClicked();
    void startButtonClicked();
    void tableDataChanged();

protected:
    virtual void CreateQtPartControl(QWidget *parent) override;
    virtual void SetFocus() override {}; // required by blueberry
    bool isValidSelection();
    BoneDensityFunctor createDensityFunctorFromGui();

    Ui::MaterialMappingViewControls m_Controls;
    CalibrationDataModel m_CalibrationDataModel;

    std::unique_ptr<Testing::Runner> m_TestRunner;
};
