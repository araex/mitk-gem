#pragma once

#include <memory>
#include <QStandardItemModel>
#include <QString>

#include "BoneDensityParameters.h"

class CalibrationDataModel : public QObject {
    Q_OBJECT

    using TData = QList<std::pair<double, double>>;

public:
    CalibrationDataModel();

    // manipulation
    int appendRow(double, double);
    int appendRow(QString, QString);
    void removeRow(int);

    QAbstractItemModel *getQItemModel() const;
    BoneDensityParameters::RhoCt getFittedLine();

signals:
    void dataChanged();

public slots:
    void openLoadFileDialog();
    void openSaveFileDialog();
    void itemChanged(QStandardItem*);

private:
    void clear();
    bool isValidNumberPair(QStringList);
    void readFromFile(QString);
    void saveToFile(QString);

    std::unique_ptr <QStandardItemModel> m_ItemModel;
    TData m_Data;
};