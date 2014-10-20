/**
* Baseclass for workers
*
* TODO:
* (-)
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
    void finished(itk::DataObject::Pointer ptr, unsigned int workerId);

public:
    Worker() {
    };

    void run() {
        process();
    };
};

#endif