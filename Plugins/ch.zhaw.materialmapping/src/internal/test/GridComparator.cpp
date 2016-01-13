#include "GridComparator.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include "Statistics.h"

using namespace std;

GridComparator::GridComparator(GridComparator::UgridPtr _u0, GridComparator::UgridPtr _u1, stringstream *_out)
        : u0(_u0)
        , u1(_u1)
        , out(_out)
{
    *out << std::endl;

    // cells
    auto u0_cells = u0->GetCellData();
    auto u1_cells = u1->GetCellData();
    auto u0_numCells = u0_cells->GetNumberOfArrays();
    auto u1_numCells = u1_cells->GetNumberOfArrays();

    if(u0_numCells == u1_numCells){
        for(auto i = 0; i < u0_numCells; ++i){
            auto u0_a = u0_cells->GetArray(i);
            auto u1_a = u1_cells->GetArray(i);
            compareArrayStructure(u0_a, u1_a, "cell (id '" + std::to_string(i) + "')");
            compareArrayData(u0_a, u1_a, "cell (id '" + std::to_string(i) + "')");
        }
    } else {
        *out << "ERROR: uneven number of cell arrays: " << u0_numCells << "\t|\t" << u1_numCells << std::endl;
        *out << "ERROR: skipped cell check." << std::endl;
    }

    // points
    auto u0_points = u0->GetPointData();
    auto u1_points = u1->GetPointData();
    auto u0_numPoints = u0_points->GetNumberOfArrays();
    auto u1_numPoints = u1_points->GetNumberOfArrays();

    if(u0_numPoints == u1_numPoints){
        for(auto i = 0; i < u0_numPoints; ++i){
            auto u0_a = u0_points->GetArray(i);
            auto u1_a = u1_points->GetArray(i);
            compareArrayStructure(u0_a, u1_a, "points (id '" + std::to_string(i) + "')");
            compareArrayData(u0_a, u1_a, "points (id '" + std::to_string(i) + "')");
        }
    } else {
        *out << "ERROR: uneven number of point arrays: " << u0_numPoints << "\t|\t" << u1_numPoints << std::endl;
        *out << "ERROR: skipped points check." << std::endl;
    }
}

void GridComparator::compareArrayStructure(vtkDataArray *_a0, vtkDataArray *_a1, std::string _prefix) {
    *out << _prefix << " STARTED array structure comparison" << std::endl;

    string a0_name = _a0->GetName();
    string a1_name = _a1->GetName();

    if(a0_name == a1_name){
        *out << _prefix << ".1 PASS: same named array '" << a0_name << "'." << std::endl;
    } else {
        *out << _prefix << ".1 FAIL: wrong array name '" << a0_name << "' vs '" << a1_name << "'." << std::endl;
    }

    auto a0_comp = _a0->GetNumberOfComponents();
    auto a1_comp = _a1->GetNumberOfComponents();

    if(a0_comp == a1_comp){
        *out << _prefix << ".2 PASS: same number of components '" << a0_comp << "'." << std::endl;
    } else {
        *out << _prefix << ".2 FAIL: wrong number of components '" << a0_comp << "' vs '" << a1_comp << "'." << std::endl;
    }

    auto a0_ntup = _a0->GetNumberOfTuples();
    auto a1_ntup = _a1->GetNumberOfTuples();

    if(a0_ntup == a1_ntup){
        *out << _prefix << ".3 PASS: same number of tuples '" << a0_ntup << "'." << std::endl;
    } else {
        *out << _prefix << ".3 FAIL: wrong number of tuples '" << a0_ntup << "' vs '" << a1_ntup << "'." << std::endl;
    }
}

void GridComparator::compareArrayData(vtkDataArray *_a0, vtkDataArray *_a1, std::string _prefix) {
    *out << _prefix<< " STARTED data comparison '" <<  std::string(_a0->GetName()) << "' | '" << std::string(_a1->GetName()) << "'." << std::endl;
    auto numberOfErrors = 0;

    auto ncomp = _a0->GetNumberOfComponents();
    auto ntup = _a1->GetNumberOfTuples();

    assert(ncomp < 10);
    double a0_buf[10], a1_buf[10];
    std::vector<double> diff;
    for(auto j = 0; j<ntup; ++j){
        _a0->GetTuple(j, a0_buf);
        _a1->GetTuple(j, a1_buf);

        for(auto k = 0; k < ncomp; ++k){
            double v0 = a0_buf[k];
            double v1 = a1_buf[k];
            auto d = std::abs(v0 - v1);
            if(d > 0.00001){
                diff.push_back(std::abs(d));
                ++numberOfErrors;
            }
        }
    }

    if(numberOfErrors == 0){
        *out << _prefix << " PASS: data is equal" << std::endl;
    } else {
        auto errorPerc = 100.0 / ntup * numberOfErrors;
        *out << _prefix << " FAIL: is not equal" << std::endl;
        *out << _prefix << " \t " << numberOfErrors << " / " << ntup << " (" << errorPerc << "% error rate)" << std::endl;
        *out << _prefix << " \t mean error:\t" << Testing::getMean(diff) << std::endl;
        *out << _prefix << " \t median error:\t" << Testing::getMedian(diff) << std::endl;
        *out << _prefix << " \t stddev error:\t" << Testing::getStdDev(diff) << std::endl;
        *out << _prefix << " \t min error:\t" << Testing::getMin(diff) << std::endl;
        *out << _prefix << " \t max error:\t" << Testing::getMax(diff) << std::endl;
    }
}

void GridComparator::printBuf(double *_buf, int _comp) {
    *out << "[ ";
    for(auto i = 0; i < _comp; ++i){
        *out << _buf[i] << " ";
    }
    *out << "]";
}