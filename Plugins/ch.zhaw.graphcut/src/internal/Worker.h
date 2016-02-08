/**
 *  MITK-GEM: Graphcut Plugin
 *
 *  Copyright (c) 2016, Zurich University of Applied Sciences, School of Engineering, T. Fitze, Y. Pauchard
 *
 *  Licensed under GNU General Public License 3.0 or later.
 *  Some rights reserved.
 */

#ifndef __Worker_h_
#define __Worker_h_

#include <QObject>
#include <QRunnable>

#include <itkDataObject.h>

class Worker : public QObject, public QRunnable {
    Q_OBJECT

public slots:
    virtual void process() = 0;

    signals:
    void started(unsigned int workerId);
    void progress(float progress, unsigned int workerId);
    void finished(itk::DataObject::Pointer ptr, unsigned int workerId);

public:
    Worker() {
    };

    void run() {
        process();
    };

    virtual void itkProgressCommandCallback(float progress) = 0;
};

#endif