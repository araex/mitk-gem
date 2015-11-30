#pragma once

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

    // IO
    void openLoadFileDialog();
    void openSaveFileDialog();

    QAbstractItemModel *getQItemModel() const;

    BoneDensityParameters::RhoCt getFittedLine();

signals:
    void dataChanged();

protected slots:
    void itemChanged(QStandardItem*);

private:
    void clear();
    bool isValidNumberPair(QStringList);
    void readFromFile(QString);
    void saveToFile(QString);

    std::unique_ptr <QStandardItemModel> m_ItemModel;
    TData m_Data;
};