#include <string>
#include <QMessageBox>
#include <QTextStream>
#include <mitkLogMacros.h>

#include <vnl/algo/vnl_lsqr.h>
#include <vnl/vnl_sparse_matrix_linear_system.h>
#include <vnl/vnl_least_squares_function.h>



#include "CalibrationDataModel.h"

#include <stdexcept>

using namespace std;

CalibrationDataModel::CalibrationDataModel() {
    m_ItemModel = unique_ptr<QStandardItemModel>(new QStandardItemModel(0, 2));

    // listeners
    QObject::connect(getQItemModel(), SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)));

    // init table
    setUnit(Unit::mgHA_cm3);
    clear();
}

void CalibrationDataModel::setUnit(QString _s) {
    if(_s == "mgHA/cm³"){
        setUnit(Unit::mgHA_cm3);
    } else if(_s == "gHA/cm³"){
        setUnit(Unit::gHA_cm3);
    } else {
        QMessageBox::warning(0, "Invalid conversion unit!", "Failed to set conversion unit: " + _s + ".");
    }
}

void CalibrationDataModel::setUnit(Unit _u) {
    m_SelectedUnit = _u;

    auto s = QString::fromUtf8(getUnitString().c_str());
    QString header = "Bone density [ " + s + " ]";

    m_ItemModel->setHorizontalHeaderItem(0, new QStandardItem(QString("Intensity [ HU ]")));
    m_ItemModel->setHorizontalHeaderItem(1, new QStandardItem(QString(header)));
}

void CalibrationDataModel::itemChanged(QStandardItem *_item){
    auto row = _item->row();
    auto first = m_ItemModel->item(row, 0);
    auto second = m_ItemModel->item(row, 1);

    if(first == nullptr || second == nullptr){ // default row contains only 1 item
        return;
    }

    auto isLastRow = (row == (m_ItemModel->rowCount() - 1));
    bool aIsNumber = false, bIsNumber = false;
    auto a = first->text().toDouble(&aIsNumber);
    auto b = second->text().toDouble(&bIsNumber);

    if(aIsNumber && bIsNumber) {
        if(isLastRow){
            m_ItemModel->appendRow(new QStandardItem("")); // add empty row
            m_Data.push_back(make_pair(0,0)); // insert data
        }

        m_Data[row].first = a;
        m_Data[row].second = b;
    } else {
        if(!aIsNumber){
            first->setText("");
            return;
        }
        if(!bIsNumber){
            second->setText("");
            return;
        }
    }

    emit dataChanged();
}

int CalibrationDataModel::appendRow(double _a, double _b) {
    return appendRow(QString::number(_a), QString::number(_b));
}

int CalibrationDataModel::appendRow(QString _a, QString _b) {
    auto row = m_Data.size();
    auto a = new QStandardItem(_a);
    auto b = new QStandardItem(_b);
    m_ItemModel->setItem(row, 0, a);
    m_ItemModel->setItem(row, 1, b);
    return row;
}

void CalibrationDataModel::removeRow(int _idx) {
    m_ItemModel->removeRows(_idx, 1);
    m_Data.removeAt(_idx);
}

QAbstractItemModel *CalibrationDataModel::getQItemModel() const {
    return m_ItemModel.get();
}

void CalibrationDataModel::clear() {
    m_ItemModel->removeRows(0, m_ItemModel->rowCount());
    m_Data.clear();
    m_ItemModel->appendRow(new QStandardItem("")); // add empty row
}

BoneDensityParameters::RhoCt CalibrationDataModel::getFittedLine() const {
    vnl_sparse_matrix<double> A(m_Data.size(), 2);
    vnl_vector<double> b(m_Data.size());

    auto factor = 1.0;
    m_SelectedUnit == Unit::mgHA_cm3 ? factor = 1000.0 : 1.0;
    for(auto i=0; i < m_Data.size(); ++i){
        A(i, 0) = m_Data[i].first;
        A(i, 1) = 1;
        b[i] = m_Data[i].second / factor;
    }
    vnl_sparse_matrix_linear_system<double> ls(A,b);
    vnl_vector<double> x(2);
    x[0] = x[1] = 0.0;
    vnl_lsqr lsqr(ls);
    lsqr.minimize(x);

    return BoneDensityParameters::RhoCt(x[0], x[1]);
}

std::string CalibrationDataModel::getUnitString() const {
    switch(m_SelectedUnit){
        case Unit::mgHA_cm3:
            return "mgHA/cm³";
        case Unit::gHA_cm3:
            return "gHA/cm³";
    }
}

TiXmlElement* CalibrationDataModel::serializeToXml() const {
    auto root = new TiXmlElement("Calibration");
    root->SetAttribute("unit", getUnitString());

    for(auto &pair : m_Data){
        auto entry = new TiXmlElement("DataPoint");
        entry->SetDoubleAttribute("HU", pair.first);
        entry->SetDoubleAttribute("rho", pair.second);
        root->LinkEndChild(entry);
    }

    return root;
}

void CalibrationDataModel::loadFromXml(TiXmlElement *_root) {
    std::string unit;
    auto r = _root->QueryStringAttribute("unit", &unit);
    if(r != TIXML_SUCCESS){
        QMessageBox::warning(0, "invalid file", "Invalid file format: Could not read calibration unit.");
    }
    setUnit(QString::fromUtf8(unit.c_str()));

    std::vector<std::pair<double, double>> measurements;
    double valHu, valRho;
    for(auto child = _root->FirstChildElement("DataPoint"); child; child = child->NextSiblingElement() ){
        auto r0 = child->QueryDoubleAttribute("HU", &valHu);
        auto r1 = child->QueryDoubleAttribute("rho", &valRho);
        if (r0 == TIXML_SUCCESS && r1 == TIXML_SUCCESS){
            measurements.push_back(std::make_pair(valHu, valRho));
        } else {
            QMessageBox::warning(0, "failed to load file", "invalid file format.");
            return;
        }
    }

    clear();
    for(auto &pair : measurements){
        appendRow(QString::number(pair.first), QString::number(pair.second));
    }
}

bool CalibrationDataModel::isValidNumberPair(QStringList _stringlist) {
    if(_stringlist.size() != 2){
        return false;
    }

    bool isNumber = true;
    for(auto i = 0; i < _stringlist.size(); ++i){
        bool b;
        _stringlist.at(i).toDouble(&b);
        isNumber = isNumber && b;
    }
    return isNumber;
}

bool CalibrationDataModel::hasExpectedValueRange() const {
    if(m_ItemModel->rowCount() <= 2){ // need more than 1 entry
        return true;
    }

    auto factor = 1.0;
    m_SelectedUnit == Unit::mgHA_cm3 ? factor = 1000.0 : 1.0;

    auto lo = std::numeric_limits<double>::max();
    auto hi = std::numeric_limits<double>::lowest();
    for(auto i=0; i < m_Data.size(); ++i){
        double value = m_Data[i].second / factor;
        hi = std::max(hi, value);
        lo = std::min(lo, value);
    }
    auto validRange = lo > -5.0 && hi < 5.0;
    auto delta = hi - lo;
    auto validDelta = delta > 0.0001;
    return validRange && validDelta;
}