
#ifndef INPAINTING_DECTECTION_CONNECTED_COMPONENTS_H
#define INPAINTING_DECTECTION_CONNECTED_COMPONENTS_H


#include <unordered_set>
#include <random>


#include "utils.h"

struct EquivalenceTable {

    inline void add(int i, int j) {
        int l = std::max(i, j) + 1;
        while (table.size() < l) {
            table.emplace_back();
        }
        table[i].insert(j);
        table[j].insert(i);
    }

    void compute_unique_labels(int n_labels) {
        int current_label = 0;
        int max = std::numeric_limits<int>::max();
        while (table.size() < n_labels) {
            table.emplace_back();
        }
        unique_labels.assign(n_labels, max);

        for (int l = 0; l < n_labels; l++) {
            if (unique_labels[l] == max) {
                unique_labels[l] = current_label++;
            }
            for (int lb : table[l]) {
                int m = std::min(unique_labels[l], unique_labels[lb]);
                unique_labels[l] = m;
                unique_labels[lb] = m;
            }
        }


    }

    inline int operator()(int l) {
        return unique_labels[l];
    }

    vector<std::unordered_set<int>> table;
    vector<int> unique_labels;
};

template<class T>
class ConnectedComponentsFinder {


public:
    ConnectedComponentsFinder(const Mat_<T> &image) :
            image(image), labels(image.size()), current_label(0) {}

    Mat_<int> get_connected_components() {

        _first_pass();
        _second_pass();

        return labels;
    }

    vector<int> get_areas_as_vector() {
        vector<int> areas(max_unique_label + 1, 0);
        for (int i = 0; i < labels.rows; i++) {
            for (int j = 0; j < labels.cols; j++) {
                areas[labels(i, j)]++;
            }
        }

        return areas;
    }


private:


    void _init_first_row_and_col() {

        labels(0, 0) = 0;
        for (int i = 1; i < image.rows; i++) {
            if (image(i - 1, 0) == image(i, 0)) {
                labels(i, 0) = labels(i - 1, 0);
            } else {
                labels(i, 0) = ++current_label;
            }
        }

        for (int j = 1; j < image.cols; j++) {
            if (image(0, j) == image(0, j - 1)) {
                labels(0, j) = labels(0, j - 1);
            } else {
                labels(0, j) = ++current_label;
            }
        }
    }

    void _first_pass() {

        _init_first_row_and_col();
        for (int i = 1; i < image.rows; i++) {
            for (int j = 1; j < image.cols; j++) {
                bool eq_left = image(i, j) == image(i, j - 1);
                bool eq_top = image(i, j) == image(i - 1, j);
                if (eq_left && eq_top) {
                    eq_table.add(labels(i - 1, j), labels(i, j - 1));
                    labels(i, j) = labels(i - 1, j);
                } else if (eq_top) {
                    labels(i, j) = labels(i - 1, j);
                } else if (eq_left) {
                    labels(i, j) = labels(i, j - 1);
                } else {
                    labels(i, j) = ++current_label;
                }
            }
        }
    }

    void _second_pass() {
        eq_table.compute_unique_labels(current_label + 1);
        max_unique_label = -1;
        for (int &label : labels) {
            label = eq_table(label);
            max_unique_label = std::max(label, max_unique_label);
        }
    }

    const Mat_<T> &image;
    Mat_<int> labels;
    int current_label, max_unique_label;
    EquivalenceTable eq_table;
};

inline Mat_<Vec3b> connected_comnents_to_image(const Mat_<int> &labels) {
    auto it = std::max_element(labels.begin(), labels.end());
    vector<Vec3b> colors(*it + 1);
    std::random_device r;
    std::default_random_engine generator(r());
    std::uniform_int_distribution<uchar> hue_dist(0, 255);
    for (Vec3b &c : colors) {
        c = Vec3b(hue_dist(generator), 255, 255);
    }

    Mat_<Vec3b> result(labels.size());
    for (int i = 0; i < result.rows; i++) {
        for (int j = 0; j < result.cols; j++) {
            result(i, j) = colors[labels(i, j)];
        }
    }

    cv::cvtColor(result, result, cv::COLOR_HSV2BGR);
    return result;
}

inline void display_labels(const Mat_<int> &labels) {
    display_blocking(connected_comnents_to_image(labels));
}


#endif //INPAINTING_DECTECTION_CONNECTED_COMPONENTS_H
