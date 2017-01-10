//
// Created by antoinewdg on 12/16/16.
//

#include "misc.h"

template Mat_<float> compute_patch_std_dev<Vec3b>(Mat_<Vec3b> m, int P);


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