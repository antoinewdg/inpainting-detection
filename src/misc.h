//
// Created by antoinewdg on 12/16/16.
//

#ifndef INPAINTING_DECTECTION_MISC_H
#define INPAINTING_DECTECTION_MISC_H

#include <algorithm>
#include <fstream>
#include "utils.h"

template<class T>
Mat_<float> compute_patch_variance(Mat_<T> m, int P) {
    Mat_<float> variance(m.size(), 0);
    for (int i = 2; i < m.rows - 2; i++) {
        for (int j = 2; j < m.cols - 2; j++) {
            Rect r(j - 2, i - 2, P, P);
            cv::Scalar mean = cv::mean(m(r));
            variance(i, j) = float(cv::norm(m(r) - mean, cv::NORM_L2SQR));
        }
    }
    variance /= (P * P);
    return variance;
}

template<class T>
void save_to_txt(string filename, Mat_<T> m) {
    std::ofstream out(filename);
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            out << m(i, j) << " ";
        }
        out << endl;
    }

    out.close();
}

float estimate_noise_level(const Mat_<Vec3b> &image);

float estimate_glocal_noise_level(const Mat_<Vec3b> &image, int w = 50);

Mat_<float> estimate_local_noise_level(Mat_<Vec3b> image, int w);

Mat_<bool> hysteresis_filter(const Mat_<bool> &in_high, const Mat_<bool> &in_low);

#endif //INPAINTING_DECTECTION_MISC_H
