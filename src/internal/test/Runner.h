#pragma once

#include <QObject>

#include <mitkUnstructuredGrid.h>

namespace Testing {
    class Runner : public QObject{
        Q_OBJECT
    public:
        void compareGrids(mitk::UnstructuredGrid::Pointer, mitk::UnstructuredGrid::Pointer);

    protected slots:
        void runUnitTests();
    };
}