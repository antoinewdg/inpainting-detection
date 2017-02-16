#include "detector.h"

void Detector::_perform_patch_match() {

    OffsetMap offset_map(m_image.size());
    DistanceMap distance_map(m_image.size());
    PatchServer patches(m_image.size(), P);
    PatchDistance<P> patch_distance(m_image, MIN_PATCH_OFFSET);

    Matcher matcher(patches, patches, patch_distance, offset_map, distance_map);
    matcher.initialize_offset_map_randomly();
    matcher.iterate_n_times(5);

    for (Vec2i p : patches) {
        Vec2i q = p + offset_map(p);
        int d = patch_distance(q, p);
        if (d < distance_map(q)) {
            offset_map(q) = p - q;
            distance_map(q) = d;
        }
    }

    matcher.iterate_n_times(5);


    m_offset_map = offset_map.to_mat();
    m_distance_map = distance_map.to_mat();


    write_to_yaml(_get_out_filename("offsets", "yml"), m_offset_map);
    write_to_yaml(_get_out_filename("distances", "yml"), m_distance_map);
//    save_to_txt(_get_out_filename("distances", "txt"), m_distance_map);

}

void Detector::_compute_or_load_patch_match() {
    if (!m_distance_map.empty()) {
        return;
    }
    string offsets_name = _get_out_filename("offsets", "yml");
    string distances_name = _get_out_filename("distances", "yml");

    if (_file_exists(offsets_name) && _file_exists(distances_name)) {
        m_offset_map = load_from_yaml<Vec2i>(offsets_name);
        m_distance_map = load_from_yaml<int>(distances_name);
        return;
    }

    _perform_patch_match();
}

bool Detector::_is_patch_symmetric(int i, int j) {
    if (m_variance(i, j) < VARIANCE_THRESHOLD) {
        return false;
    }
    Vec2i p(i, j);
    Vec2i q = m_offset_map(p) + p;
    int a = p[0] - q[0];
    int b = p[1] - q[1];
    if (a * a + b * b <= (MIN_PATCH_OFFSET + 5) * (MIN_PATCH_OFFSET + 5)) {
        return false;
    }
    Vec2i p_bis = m_offset_map(q) + q;
    return p_bis == p;
}

void Detector::_perform_symmetry_map() {
    m_symmetry_map = Mat_<bool>(m_offset_map.size(), 0);
    for (int i = 2; i < m_offset_map.rows - 2; i++) {
        for (int j = 2; j < m_offset_map.cols - 2; j++) {
            m_symmetry_map(i, j) = uchar(255) * _is_patch_symmetric(i, j);
        }
    }


    string filename = _get_out_filename("symmetry_map", "png");
//    cv::imwrite(filename, 255 * m_symmetry_map);


}

inline string n_to_str(int n, int n_digits) {
    int k = 0;
    for (int m = n; m > 0; m /= 10) {
        k++;
    }

    return string(n_digits - k, '0') + std::to_string(n);


}

void Detector::_perform_granulometry() {
//    Rect r(2, 2, m_symmetry_map.cols - 4, m_symmetry_map.rows - 4);
//    Mat_<bool> sym_map = m_symmetry_map(r);

    string sym_name = _get_out_filename("granulometry", "png", "_00");
    cv::imwrite(sym_name, 255 * m_symmetry_map);

    std::ofstream out(_get_out_filename("granulometry", "txt"));

    int M = cv::countNonZero(m_symmetry_map);

    Mat_<uchar> opening;
    auto struct_el = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
    cv::morphologyEx(m_symmetry_map, opening, cv::MORPH_OPEN, struct_el);
    string file_name = _get_out_filename("granulometry", "png", "_010");
    cv::imwrite(file_name, 255 * opening);

    Mat_<bool> res;
    cv::medianBlur(opening, res, 5);
    file_name = _get_out_filename("granulometry", "png", "_015");
    cv::imwrite(file_name, 255 * res);

    cv::medianBlur(opening, res, 7);
    file_name = _get_out_filename("granulometry", "png", "_017");
    cv::imwrite(file_name, 255 * res);

    cv::medianBlur(opening, res, 9);
    file_name = _get_out_filename("granulometry", "png", "_019");
    cv::imwrite(file_name, 255 * res);
//    cv::morphologyEx(opening, opening, cv::MORPH_CLOSE, struct_el);
//    file_name = _get_out_filename("granulometry", "png", "_011");
//    cv::imwrite(file_name, 255 * opening);
//
//    opening = hysteresis_connected(opening, m_connected_components);
//    file_name = _get_out_filename("granulometry", "png", "_020");
//    cv::imwrite(file_name, 255 * opening);
//
    file_name = _get_out_filename("granulometry", "png", "_099");
    cv::imwrite(file_name, m_image);
//
//    struct_el = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(11, 11));
//    cv::morphologyEx(m_symmetry_map, opening, cv::MORPH_OPEN, struct_el);
//    file_name = _get_out_filename("granulometry", "png", "_030");
//    cv::imwrite(file_name, 255 * opening);

//    struct_el = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
//    cv::morphologyEx(opening, opening, cv::MORPH_OPEN, struct_el);
//    file_name = _get_out_filename("granulometry", "png", "_020");
//    cv::imwrite(file_name, 255 * opening);
//
//    cv::morphologyEx(opening, opening, cv::MORPH_CLOSE, struct_el);
//    file_name = _get_out_filename("granulometry", "png", "_021");
//    cv::imwrite(file_name, 255 * opening);


//    for (int n = 1; n < 10; n++) {
//        Mat_<uchar> opening;
//        auto struct_el = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * n + 1, 2 * n + 1));
//        cv::morphologyEx(m_symmetry_map, opening, cv::MORPH_OPEN, struct_el);
//        float c = float(cv::countNonZero(opening)) / M;
//        out << " " << c;
//        if (c != 0) {
//            string file_name = _get_out_filename("granulometry", "png", "_" + n_to_str(n, 2) + '0');
//            cv::imwrite(file_name, 255 * opening);
//        }
//    }


    out.close();
}

void Detector::_perform_suspicious_zones() {
    int d = 2 * 5 + 1;
    auto struct_el = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(d, d));
    Mat_<bool> opening;
    cv::morphologyEx(m_symmetry_map, opening, cv::MORPH_OPEN, struct_el);
    m_suspicious_zones = hysteresis_filter(opening, m_symmetry_map);

    Mat_<Vec3b> image(m_image.size());
    for (int i = 2; i < image.rows - 2; i++) {
        for (int j = 2; j < image.cols - 2; j++) {
            image(i, j) = m_image(i, j);
            if (!m_suspicious_zones(i, j)) {
                image(i, j) = image(i, j) - Vec3b(70, 0, 70);
            }
        }
    }
    string name = _get_out_filename("suspicious", "png");
    cv::imwrite(name, image);
    name = _get_out_filename("symmetry_map_clean", "png");
    cv::imwrite(name, 255 * m_suspicious_zones);
}

void Detector::_perform_distance_hist() {
    std::ofstream symmetric(_get_out_filename("hist_symmetric", "txt"));
    std::ofstream asymmetric(_get_out_filename("hist_asymmetric", "txt"));
    std::ofstream all(_get_out_filename("hist_all", "txt"));

    bool found_sym = false;
    for (int i = P / 2; i < m_distance_map.rows - P / 2; i++) {
        for (int j = P / 2; j < m_distance_map.cols - P / 2; j++) {
            if (m_suspicious_zones(i, j)) {
                symmetric << m_distance_map(i, j) << " ";
                found_sym = true;
            } else {
                asymmetric << m_distance_map(i, j) << " ";
            }
            all << m_distance_map(i, j) << " ";
        }
    }

    symmetric.close();
    asymmetric.close();

    if (!found_sym) {
        std::remove(_get_out_filename("hist_pos", "txt").c_str());
    }
//    neg_out.close();
}

void Detector::_perform_std_dev() {
    m_variance = compute_patch_variance(m_image, P);
//    save_to_txt(_get_out_filename("variance"),
//                m_variance(_get_patches_rect()));
}

void Detector::_perform_noise_estimation() {
    float noise = estimate_glocal_noise_level(m_image);
    string filename = _get_out_filename("noise", "txt");
    std::ofstream out(filename);
    out << noise;
    out.close();
}

void Detector::_perform_connected_components() {
    ConnectedComponentsFinder<Vec2i> finder(m_offset_map);
    m_connected_components = finder.get_connected_components();
    auto image = connected_comnents_to_image(m_connected_components);
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            if (!m_symmetry_map(i, j)) {
                image(i, j) = Vec3b(0, 0, 0);
            }
        }
    }
    string filename = _get_out_filename("connected_components", "png");
    cv::imwrite(filename, image);
}

void Detector::_perform_distance_map() {
    Mat_<Vec3b> out(m_image.size(), Vec3f(0, 0, 0));

    float max_d = 0.f;
    for (int i = 2; i < m_distance_map.rows - 2; i++) {
        for (int j = 2; j < m_distance_map.cols - 2; j++) {
            float d = std::sqrt(m_distance_map(i, j));
            max_d = std::max(max_d, d);
            out(i, j) = Vec3f(d, d, d);
        }
    }

    out /= (max_d / 255.f);

    for (int i = 2; i < m_distance_map.rows - 2; i++) {
        for (int j = 2; j < m_distance_map.cols - 2; j++) {
            if (out(i, j)[0] == 0 && m_variance(i, j) >= VARIANCE_THRESHOLD) {
                out(i, j) = Vec3f(255, 0, 255);
            }
        }
    }

    string name = _get_out_filename("suspicious", "png", "_dm");
    cv::imwrite(name, out);
}

int Detector::_count_coherence(int i, int j) {
    int c = 0;
    auto offset = m_offset_map(i, j);
    for (int k = -2; k <= 2; k++) {
        for (int l = -2; l <= 2; l++) {
            if (m_offset_map(i + k, j + l) == offset && m_symmetry_map(i + k, j + l)) {
                c += 1;
            }
        }
    }
    return c;
}

void Detector::_perform_symmetry_coherence() {
    Mat_<float> coherence(m_symmetry_map.size(), 0.f);
    for (int i = 2; i < m_distance_map.rows - 2; i++) {
        for (int j = 2; j < m_distance_map.cols - 2; j++) {
            coherence(i, j) = float(_count_coherence(i, j)) / 25;
        }
    }

    string name = _get_out_filename("coherence");
    cv::imwrite(name, 255 * coherence);
}


float get_norm_penalization(Vec2i o) {
    float n = std::sqrt(float(o[0]) * o[0] + o[1] * o[1]);
    return 1.f / (1.f + std::exp(0.146f * (25.f - n)));
}

float get_angle_penalization(Vec2i o) {
    if (std::abs(o[0]) <= 1 || std::abs(o[1]) <= 1) {
        return 0.5f;
    }
    return 1.f;
}

float get_penalization(Vec2i o) {
    return get_norm_penalization(o) * get_angle_penalization(o);
}

void Detector::_perform_offset_hist() {
//    OffsetHistogram oh(m_offset_map);
//    Mat_<float> hist = oh.build_hist();
//    string name = _get_out_filename("offset_hist");
//    cv::imwrite(name, 255 * hist);

    int d = 2 * 1 + 1;
    auto struct_el = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(d, d));
    Mat_<bool> opening;
    cv::morphologyEx(m_symmetry_map, opening, cv::MORPH_OPEN, struct_el);

    Mat_<Vec2i> sym_offsets(m_offset_map.size(), Vec2i(0, 0));
    for_each_patch<5>(m_offset_map, [&](int i, int j) {
        if (opening(i, j)) {
            sym_offsets(i, j) = m_offset_map(i, j);
        }
    });

    OffsetHistogram ohb(sym_offsets);
    auto hist = ohb.build_hist();

//    for_each_pixel(hist, [&](int i, int j, ))
    auto name = _get_out_filename("offset_hist_sym");
    cv::imwrite(name, 255 * hist);

    Mat_<bool> detection_mask(m_image.size(), false);

    auto values = ohb.get_sorted_values();
    for (int k = 0; k < std::min(int(values.size()), 5); k++) {
        auto o = values[k].first;

        auto rel = _check_bidir_offset_relevance(o);
        if (k + 1 < values.size()) {
            auto o2 = values[k + 1].first;
            if (std::abs(o[0] - o2[0]) + std::abs(o[1] - o2[1]) == 1) {
                rel = rel + _check_bidir_offset_relevance(o2);
                k++;
            }
        }
        string fname = _get_out_filename("relevant_offset", ".png", "_" + std::to_string(k));
        cv::imwrite(fname, 255 * rel);
        ConnectedComponentsFinder<bool> finder(rel);
        auto cc = finder.get_connected_components();
        vector<int> areas = finder.get_areas_as_vector_without_bg(false);
        vector<int> indexes(areas.size(), 0);
        for (int i = 0; i < indexes.size(); i++) { indexes[i] = i; }
        auto it = std::max_element(indexes.begin(), indexes.end(), [&](int i, int j) { return areas[i] < areas[j]; });

//        cout << o << "  " << get_angle_penalization(o) << " area: " << areas[*it] << endl;
//        display_blocking(rel);
        float alpha = get_penalization(o);
        if (alpha * areas[*it] > MIN_CONNECTED_AREA) {

            for_each_pixel(detection_mask, [&](int i, int j, bool &d) {
                if (cc(i, j) == *it) {
                    detection_mask(i, j) = true;
                    detection_mask(i + o[0], j + o[1]) = true;
                }
            });
        }

    }

//    display_blocking(detection_mask);

    name = _get_out_filename("detection_mask");
//    display_blocking(detection_mask);
    cv::imwrite(name, detection_mask * 255);

//    std::ofstream out(name);
//
//    for (auto p : values) {
//        out << p.second << " ";
//    }
//    out.close();
}

Mat_<bool> Detector::_check_offset_relevance(int k, Vec2i o) {
    PatchDistance<P> patch_distance(m_image, MIN_PATCH_OFFSET);
    Mat_<bool> relevance(m_offset_map.size(), false);
    int i_min = std::max(2, 2 - o[0]);
    int i_max = std::min(m_image.rows - 2 - o[0], m_image.rows - 2);
    int j_min = std::max(2, 2 - o[1]);
    int j_max = std::min(m_image.cols - 2 - o[1], m_image.cols - 2);

    for (int i = i_min; i < i_max; i++) {
        for (int j = j_min; j < j_max; j++) {
            Vec2i p(i, j);
            Vec2i q = p + o;
            float d = patch_distance(p, q, 4 * m_distance_map(p));
            relevance(i, j) = d / m_distance_map(p) <= 1;
        }
    }

//    display_blocking(relevance);
    return relevance;
}

Mat_<bool> Detector::_check_bidir_offset_relevance(Vec2i o) {
    auto rel = _check_offset_relevance(0, o);
    auto rev = _check_offset_relevance(0, -o);

    for_each_pixel(rel, [&](int i, int j, bool &b) {
        if (b && !rev(Vec2i(i, j) + o)) {
            b = false;
        }
    });

    return rel;
}