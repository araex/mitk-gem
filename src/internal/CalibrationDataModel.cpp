#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

#include <mitkLogMacros.h>

#include <vnl/algo/vnl_lsqr.h>
#include <vnl/vnl_sparse_matrix_linear_system.h>
#include <vnl/vnl_least_squares_function.h>

#include "CalibrationDataModel.h"

using namespace std;

CalibrationDataModel::CalibrationDataModel() {
    m_ItemModel = unique_ptr<QStandardItemModel>(new QStandardItemModel(0, 2));

    // listeners
    QObject::connect(getQItemModel(), SIGNAL(itemChanged(QStandardItem*)), this, SLOT(itemChanged(QStandardItem*)));

    // init table
    m_ItemModel->setHorizontalHeaderItem(0, new QStandardItem(QString("Intensity [HU]")));
    m_ItemModel->setHorizontalHeaderItem(1, new QStandardItem(QString("?")));
    clear();
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

void CalibrationDataModel::openLoadFileDialog() {
    auto filename = QFileDialog::getOpenFileName(0, tr("Open measurement file"), "", tr("measurement file (*.txt)"));
    if(!filename.isNull()){
        MITK_INFO << "loading measurments from file: " << filename.toUtf8().constData();
        readFromFile(filename);
    } else {
        MITK_INFO << "canceled file open dialog.";
    }
}

void CalibrationDataModel::openSaveFileDialog() {
    auto filename = QFileDialog::getSaveFileName(0, tr("Save measurement file"), "", tr("measurement file (*.txt)"));
    if(!filename.isNull()){
        MITK_INFO << "saving measurments to file: " << filename.toUtf8().constData();
        saveToFile(filename);
    } else {
        MITK_INFO << "canceled file save dialog.";
    }
}

void CalibrationDataModel::clear() {
    m_ItemModel->removeRows(0, m_ItemModel->rowCount());
    m_Data.clear();
    m_ItemModel->appendRow(new QStandardItem("")); // add empty row
}

CalibrationDataModel::Line CalibrationDataModel::getFittedLine() {
    vnl_sparse_matrix<double> A(m_Data.size(), 2);
    vnl_vector<double> b(m_Data.size());

    for(auto i=0; i < m_Data.size(); ++i){
        A(i, 0) = m_Data[i].first;
        A(i, 1) = 1;
        b[i] = m_Data[i].second;
    }
    vnl_sparse_matrix_linear_system<double> ls(A,b);
    vnl_vector<double> x(2);
    x[0] = x[1] = 0.0;
    vnl_lsqr lsqr(ls);
    lsqr.minimize(x);
    return Line{x[0], x[1]};
}

void CalibrationDataModel::readFromFile(QString _path) {
    QFile file(_path);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(0, "failed to open file", file.errorString());
    }

    QTextStream in(&file);
    std::vector<std::pair<QString, QString>> measurements;
    while(!in.atEnd()) {
        QString line = in.readLine();
        if(line.isEmpty()){
            continue;
        }
        QStringList fields = line.split(" ");
        if(isValidNumberPair(fields)){
            measurements.push_back(std::make_pair(fields.at(0), fields.at(1)));
        } else {
            QMessageBox::warning(0, "failed to load file", "invalid file format.");
            file.close();
            return;
        }
    }
    file.close();

    clear();
    for(auto &pair : measurements){
        appendRow(pair.first, pair.second);
    }
}

void CalibrationDataModel::saveToFile(QString _path) {
    QFile file(_path);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(0, "failed to open file", file.errorString());
    }

    QTextStream out(&file);
    for(auto &pair : m_Data){
        out << pair.first << " " << pair.second << '\n';
    }
    file.close();
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