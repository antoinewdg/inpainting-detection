//
// Created by antoinewdg on 12/16/16.
//

#include "misc.h"

template Mat_<float> compute_patch_variance<Vec3b>(Mat_<Vec3b> m, int P);


void _propagate_hysteresis(Mat_<bool> &out, const Mat_<bool> &in_low, int i, int j) {
    if (i < 0 || j < 0 || i >= out.rows || j >= out.cols || out(i, j) || !in_low(i, j)) {
        return;
    }
    out(i, j) = true;
    _propagate_hysteresis(out, in_low, i - 1, j);
    _propagate_hysteresis(out, in_low, i, j - 1);
    _propagate_hysteresis(out, in_low, i + 1, j);
    _propagate_hysteresis(out, in_low, i, j + 1);
}

float estimate_noise_level(const Mat_<Vec3b> &image) {
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


float estimate_glocal_noise_level(const Mat_<Vec3b> &image, int w) {
    int l = 2 * w + 1;
    Mat_<float> local_noises(image.rows / l, image.cols / l);
    for (int i = 0; i < local_noises.rows; i++) {
        for (int j = 0; j < local_noises.cols; j++) {
            cv::Rect r(j * l, i * l, l, l);
            local_noises(i, j) = estimate_noise_level(image(r));
        }
    }

    auto min_it = std::min_element(local_noises.begin(), local_noises.end());
    return *min_it;
}


Mat_<bool> hysteresis_filter(const Mat_<bool> &in_high, const Mat_<bool> &in_low) {
    Mat_<bool> out(in_high.size(), false);
    for (int i = 0; i < out.rows; i++) {
        for (int j = 0; j < out.cols; j++) {
            if (in_high(i, j)) {
                _propagate_hysteresis(out, in_low, i, j);
            }
        }
    }

    return out;
}

Mat_<float> estimate_local_noise_level(Mat_<Vec3b> image, int w) {
    Mat_<Vec3b> img_with_borders;
    Mat_<float> result(image.size());

    cv::copyMakeBorder(image, img_with_borders, w, w, w, w, cv::BORDER_REFLECT101);

    for (int i = 0; i < result.rows; i++) {
        for (int j = 0; j < result.cols; j++) {
            result(i, j) = estimate_noise_level(img_with_borders(cv::Rect(j, i, 2 * w + 1, 2 * w + 1)));
        }
    }
    return result;
}

void _propagate_hysteresis_connected(Mat_<bool> &out, const Mat_<int> &labels, int l, int i, int j) {
    if (i < 0 || j < 0 || i >= out.rows || j >= out.cols || out(i, j) || labels(i, j) != l) {
        return;
    }
    out(i, j) = true;
    _propagate_hysteresis_connected(out, labels, l, i - 1, j);
    _propagate_hysteresis_connected(out, labels, l, i, j - 1);
    _propagate_hysteresis_connected(out, labels, l, i + 1, j);
    _propagate_hysteresis_connected(out, labels, l, i, j + 1);
}

Mat_<bool> hysteresis_connected(const Mat_<bool> &symmetry_map, const Mat_<int> &labels) {
    Mat_<bool> out(symmetry_map.size(), false);
    for (int i = 0; i < out.rows; i++) {
        for (int j = 0; j < out.cols; j++) {
            if (symmetry_map(i, j)) {
                _propagate_hysteresis_connected(out, labels, labels(i, j), i, j);
            }
        }
    }
    return out;
}