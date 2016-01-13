#pragma once

#include <memory>
#include <QStandardItemModel>
#include <QString>

#include <tinyxml.h>

#include "BoneDensityParameters.h"

/**
 * Wrapper for a Qt ItemModel representing the calibration table
 */
class CalibrationDataModel : public QObject {
    Q_OBJECT

    using TData = QList<std::pair<double, double>>;

public:
    /**
     * Enum representing the unit of the entered bone density calibration values
     */
    enum class Unit {
        mgHA_cm3 = 0,
        gHA_cm3 = 1
    };

    CalibrationDataModel();
    int appendRow(double _ctVal, double _rhoVal);
    void removeRow(int _index);
    QAbstractItemModel *getQItemModel() const;

    /**
     * Set the unit from a given string. Pops a warning box if the unit is not recognized.
     */
    void setUnit(QString);
    void setUnit(Unit);
    std::string getUnitString() const;
    Unit getUnit() const {
        return m_SelectedUnit;
    }

    /**
     * Performs a sanity check whether or not the currently entered values are within an expected range for the given unit
     */
    bool hasExpectedValueRange() const;

    /**
     * Returns a fitted curve for the currently entered calibration values using a least squares fitting
     */
    BoneDensityParameters::RhoCt getFittedLine() const;

    /**
     * Serializes current GUI state to XML
     */
    TiXmlElement* serializeToXml() const;

    /**
     * Load GUI state from XML
     */
    void loadFromXml(TiXmlElement *);

signals:
    /**
     * Table content changed
     */
    void dataChanged();

public slots:
    void itemChanged(QStandardItem *);

private:
    int appendRow(QString, QString);
    void clear();
    bool isValidNumberPair(QStringList);
    void readFromFile(QString);
    void saveToFile(QString);

    Unit m_SelectedUnit;
    std::unique_ptr <QStandardItemModel> m_ItemModel;
    TData m_Data;
};