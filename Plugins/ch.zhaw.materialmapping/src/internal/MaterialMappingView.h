#pragma once

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>
#include <QFuture>

#include "ui_MaterialMappingViewControls.h"
#include "CalibrationDataModel.h"
#include "test/Runner.h"
#include "BoneDensityFunctor.h"
#include "PowerLawWidgetManager.h"

class MaterialMappingView : public QmitkAbstractView {
    Q_OBJECT

public:
    ~MaterialMappingView();

    static const std::string VIEW_ID;
    // enables building of the GUI unit testing
    static const bool TESTING = true;
    static Ui::MaterialMappingViewControls *controls;

protected slots:
    void deleteSelectedRows();
    void startButtonClicked();
    void tableDataChanged();
    void unitSelectionChanged(int);
    void compareGrids();
    void createEMorganImage();
    void loadParametersButtonClicked();
    void saveParametersButtonClicked();

protected:
    virtual void CreateQtPartControl(QWidget *parent) override;
    virtual void SetFocus() override {}; // required by blueberry
    bool isValidSelection();

    Ui::MaterialMappingViewControls m_Controls;
    CalibrationDataModel m_CalibrationDataModel;

    std::unique_ptr<Testing::Runner> m_TestRunner;
    std::unique_ptr<PowerLawWidgetManager> m_PowerLawWidgetManager;

    QFuture<void> m_WorkerFuture;
};
