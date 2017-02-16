//
// Created by antoinewdg on 2/8/17.
//

#ifndef INPAINTING_DECTECTION_OFFSET_HISTOGRAM_H
#define INPAINTING_DECTECTION_OFFSET_HISTOGRAM_H

#include <cmath>
#include "utils.h"

class OffsetHistogram {
public:
    OffsetHistogram(const Mat_<Vec2i> &offsets) :
            offsets(offsets),
            alpha(0.5f) {
        int rows = 2 * offsets.rows;
        int cols = 2 * offsets.cols;
        bins = Mat_<float>(rows, cols, 0.f);
    }

    void _process_offset(Vec2i o) {
        int a = o[0], b = o[1];
        int i = a + offsets.rows;
        int j = b + offsets.cols;
        bins(i, j)++;
    }

    Mat_<float> build_hist() {
        for_each_patch<5>(offsets, [&](int i, int j) {
            if (offsets(i, j) == Vec2i(0, 0)) {
                return;
            }
            _process_offset(offsets(i, j));
        });
        stretch_histogram(bins);


        return bins;
    }

    vector<std::pair<Vec2i, float>> get_sorted_values() {
        vector<std::pair<Vec2i, float>> values;
        for_each_pixel(bins, [&](int i, int j, float &c) {
            i -= offsets.rows;
            j -= offsets.cols;
            if (i < 0 || (i == 0 && j < 0)) { return; }
            if (c > 0) {
                values.push_back(std::make_pair(Vec2i(i, j), c));
            }
        });

        std::sort(values.begin(), values.end(), [&](std::pair<Vec2i, float> p, std::pair<Vec2i, float> q) {
            return p.second > q.second;
        });

        return values;
    };


    const Mat_<Vec2i> &offsets;
    Mat_<float> bins, variance;
    float alpha;
};

//class OffsetHistogram {
//    static constexpr float PI = 3.14159265358979323846f;
//public:
//    OffsetHistogram(const Mat_<Vec2i> &offsets) :
//            offsets(offsets),
//            alpha(0.5f),
//            n_theta_bins(30) {
//        float max_r = float(std::sqrt(float(offsets.rows) * offsets.rows + float(offsets.cols) * offsets.cols));
//        int n_r_bins = int(std::round(alpha * max_r));
//        bins = Mat_<float>(n_r_bins, n_theta_bins, 0.f);
//    }
//
//    void _process_offset(Vec2i o) {
//        float a = o[0], b = o[1];
//        float r = std::sqrt(a * a + b * b);
//        float theta = std::atan2(b, a);
//        if (theta < 0) {
//            theta += 2 * PI;
//        }
//
//        int i = int(std::round(alpha * r));
//        int j = int(std::round((theta * n_theta_bins) / (2 * PI)));
//        bins(i, j)++;
//    }
//
//    Mat_<float> build_hist() {
//        for_each_patch<5>(offsets, [&](int i, int j) {
//            if (offsets(i, j) == Vec2i(0, 0)) {
//                return;
//            }
//            _process_offset(offsets(i, j));
//        });
//
//
//        stretch_histogram(bins);
////        Mat_<float> sub_bins = bins(cv::Rect(0, 0, bins.cols / 2, bins.rows));
////
////        for_each_pixel(sub_bins, [&](int i, int j, float &v) {
////            v = bins(i, j + (n_theta_bins / 2)) * bins(i, j);
////        });
//
//        return bins;
//    }
//
//    const Mat_<Vec2i> &offsets;
//    Mat_<float> bins;
//    float alpha;
//    int n_theta_bins;
//};

#endif //INPAINTING_DECTECTION_OFFSET_HISTOGRAM_H
