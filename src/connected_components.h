
#ifndef INPAINTING_DECTECTION_CONNECTED_COMPONENTS_H
#define INPAINTING_DECTECTION_CONNECTED_COMPONENTS_H


#include <set>
#include <random>


#include "utils.h"

/**
 * Create an image with a random color for each label.
 *
 * @param labels
 * @return
 */
inline Mat_<Vec3b> connected_components_to_image(const Mat_<int> &labels) {
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


struct EquivalenceTable {


/* Find root of set containing x */
    int root(int x) {
        while (uf.size() < x + 1) { uf.emplace_back(-1); }
        int r = x;

        /* Find root */
        while (uf[r] >= 0)
            r = uf[r];

        /* Compress path to root */
        while (uf[x] >= 0) {
            int tmp = uf[x];
            uf[x] = r;
            x = tmp;
        }

        return r;
    }

/* Union of sets containing x and y */
    void add(int x, int y) {
        int m = std::max(x, y) + 1;
        while (uf.size() < m) { uf.emplace_back(-1); }
        x = root(x);
        y = root(y);

        if (x != y) {
            if (uf[x] < uf[y]) {
                uf[x] += uf[y];
                uf[y] = x;
            } else {
                uf[y] += uf[x];
                uf[x] = y;
            }
        }
    }

    int operator()(int x) {
        return root(x);
    }

    vector<int> uf;
};

/**
 * Class that helps find the connected components in an image.
 * @tparam T
 */
template<class T>
class ConnectedComponentsFinder {


public:
    ConnectedComponentsFinder(const Mat_<T> &image) :
            image(image), labels(image.size()), current_label(0) {}

    /**
     * Get the labels of the connected components.
     *
     * @return
     */
    Mat_<int> get_connected_components() {

        _first_pass();
        _second_pass();

        return labels;
    }

    /**
     * Get the area of each connected component.
     * @return
     */
    vector<int> get_areas_as_vector() {
        vector<int> areas(max_unique_label + 1, 0);
        for (int i = 0; i < labels.rows; i++) {
            for (int j = 0; j < labels.cols; j++) {

                areas[labels(i, j)]++;
            }
        }

        return areas;
    }

    /**
     * Get area of each connected component except thos of value bg.
     * @param bg
     * @return
     */
    vector<int> get_areas_as_vector_without_bg(T bg) {
        vector<int> areas(max_unique_label + 1, 0);
        for (int i = 0; i < labels.rows; i++) {
            for (int j = 0; j < labels.cols; j++) {
                if (image(i, j) == bg) { continue; }
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


#endif //INPAINTING_DECTECTION_CONNECTED_COMPONENTS_H
