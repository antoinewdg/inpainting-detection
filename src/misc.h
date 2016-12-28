//
// Created by antoinewdg on 12/16/16.
//

#ifndef INPAINTING_DECTECTION_MISC_H
#define INPAINTING_DECTECTION_MISC_H

#include <algorithm>
#include <fstream>
#include "utils.h"

template<class T>
Mat_<float> compute_patch_std_dev(Mat_<T> m, int P) {
    Mat_<float> variance(m.size(), 0);
    for (int i = 2; i < m.rows - 2; i++) {
        for (int j = 2; j < m.cols - 2; j++) {
            Rect r(j - 2, i - 2, P, P);
            cv::Scalar mean = cv::mean(m(r));
            variance(i, j) = float(cv::norm(m(r) - mean, cv::NORM_L2));
        }
    }
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

inline float estimate_noise_level(Mat_<Vec3b> &image) {
    Mat_<float> kernel(2, 2, 1.f);
    kernel(0, 1) = -1.f;
    kernel(1, 0) = -1.f;
    Mat_<Vec3f> convolution;
    cv::filter2D(image, convolution, CV_32FC3, kernel);
    Mat_<float> result(convolution.size());
    auto it = result.begin();
    for (Vec3f &c : convolution) {
        *it = cv::norm(c, cv::NORM_L2);
        it++;
    }
    auto begin = result.begin();
    auto mid = begin + ((result.rows * result.cols) / 2);
    auto end = result.end();
    std::nth_element(begin, mid, end);
    return *mid / 3.08f;
}

#endif //INPAINTING_DECTECTION_MISC_H
