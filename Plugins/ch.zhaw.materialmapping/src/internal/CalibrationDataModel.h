#pragma once

#include <memory>
#include <QStandardItemModel>
#include <QString>

#include <tinyxml.h>

#include "BoneDensityParameters.h"

class CalibrationDataModel : public QObject {
    Q_OBJECT

    using TData = QList<std::pair<double, double>>;

public:
    enum class Unit {
        mgHA_cm3 = 0,
        gHA_cm3 = 1
    };

    CalibrationDataModel();

    // manipulation
    int appendRow(double, double);
    int appendRow(QString, QString);
    void removeRow(int);
    void setUnit(QString);
    void setUnit(Unit);
    Unit getUnit(){
        return m_SelectedUnit;
    }
    std::string getUnitString();
    bool hasExpectedValueRange();

    QAbstractItemModel *getQItemModel() const;
    BoneDensityParameters::RhoCt getFittedLine();

    TiXmlElement* serializeToXml();
    void loadFromXml(TiXmlElement*);

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

    Unit m_SelectedUnit;
    std::unique_ptr <QStandardItemModel> m_ItemModel;
    TData m_Data;
};