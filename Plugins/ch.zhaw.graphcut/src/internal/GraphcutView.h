/**
 *  MITK-GEM: Graphcut Plugin
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef GraphcutView_h
#define GraphcutView_h

// MITK
#include <berryISelectionListener.h>
#include <QmitkAbstractView.h>
#include "ui_GraphcutViewControls.h"

// Utils
#include "WorkbenchUtils.h"

class GraphcutView : public QmitkAbstractView {
    Q_OBJECT

public:

    static const std::string VIEW_ID;

protected slots:
    void startButtonPressed();
    void refreshButtonPressed();
    void imageSelectionChanged();
    void workerHasStarted(unsigned int);
    void workerProgressUpdate(float progress, unsigned int id);
    void workerIsDone(itk::DataObject::Pointer, unsigned int);

protected:
    virtual void CreateQtPartControl(QWidget *parent);
    virtual void SetFocus();

    // called by QmitkFunctionality when DataManager's selection has changed
    virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
            const QList <mitk::DataNode::Pointer> &nodes);

    Ui::GraphcutViewControls m_Controls;

private:
    void updateMemoryRequirements(double memoryRequiredInBytes);
    void updateTimeEstimate(long long numberOfEdges);
    void initializeImageSelector(QmitkDataStorageComboBox *);
    void setMandatoryField(QWidget *, bool);
    void setWarningField(QWidget *, bool);
    void setErrorField(QWidget *, bool);
    void setQStyleSheetField(QWidget *, const char *, bool);
    bool isValidSelection();
    void lockGui(bool);
    unsigned int m_currentlyActiveWorkerCount;
};

#endif // GraphcutView_h
