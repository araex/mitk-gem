#pragma once

#include <vector>
#include <algorithm>
#include <cmath>

namespace Testing {
    template<class T>
    static double getMean(std::vector<T> &_v){
        auto size = _v.size();
        double sum = 0.0;
        for(const auto val : _v){
            sum += val;
        }
        return sum/size;
    }

    template<class T>
    static double getVariance(std::vector<T> &_v){
        auto size = _v.size();
        double mean = getMean(_v);
        double tmp = 0;
        for(const auto val : _v){
            tmp += (mean - val) * (mean - val);
        }
        return tmp / size;
    }

    template<class T>
    static double getStdDev(std::vector<T> &_v){
        return std::sqrt(getVariance(_v));
    }

    template<class T>
    static T getMedian(std::vector<T> _v){
        auto size = _v.size();
        std::sort(_v.begin(), _v.end());

        if (size % 2 == 0) {
            return (_v[(size / 2) - 1] + _v[size / 2]) / 2.0;
        }
        else {
            return _v[size / 2];
        }
    }

    template<class T>
    static T getMin(std::vector<T> _v){
        std::sort(_v.begin(), _v.end());

        return _v.front();
    }
    template<class T>
    static T getMax(std::vector<T> _v){
        std::sort(_v.begin(), _v.end());

        return _v.back();
    }
}