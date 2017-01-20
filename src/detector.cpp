#include "detector.h"

void Detector::_perform_patch_match() {

    OffsetMap offset_map(m_image.size());
    DistanceMap distance_map(m_image.size());
    PatchServer patches(m_image.size(), P);
    PatchDistance<P> patch_distance(m_image, 20);

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
    save_to_txt(_get_out_filename("distances", "txt"), m_distance_map);

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

void Detector::_perform_symmetry_map() {
    m_symmetry_map = Mat_<bool>(m_offset_map.size(), 0);
    for (int i = 2; i < m_offset_map.rows - 2; i++) {
        for (int j = 2; j < m_offset_map.cols - 2; j++) {
            Vec2i p(i, j);
            Vec2i q = m_offset_map(p) + p;
            Vec2i p_bis = m_offset_map(q) + q;
            m_symmetry_map(p) = uchar(255) * uchar(p == p_bis) * (m_variance(i, j) >= VARIANCE_THRESHOLD);
        }
    }


    string filename = _get_out_filename("symmetry_map", "png");
    cv::imwrite(filename, 255 * m_symmetry_map);


}

void Detector::_perform_granulometry() {
    Rect r(2, 2, m_symmetry_map.cols - 4, m_symmetry_map.rows - 4);
    Mat_<bool> sym_map = m_symmetry_map(r);

    string sym_name = _get_out_filename("symmetry_map", "png", "_" + std::to_string(0));
    cv::imwrite(sym_name, sym_map);

    std::ofstream out(_get_out_filename("granulometry", "txt"));

    int M = cv::countNonZero(sym_map);

    for (int n = 1; n < 10; n++) {
        Mat_<uchar> opening;
        auto struct_el = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * n + 1, 2 * n + 1));
        cv::morphologyEx(sym_map, opening, cv::MORPH_OPEN, struct_el);
        float c = float(cv::countNonZero(opening)) / M;
        out << " " << c;
        if (c != 0) {
            string file_name = _get_out_filename("is_mirror", "png", "_" + std::to_string(n));
            cv::imwrite(file_name, opening);
        }
    }


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
}

void Detector::_perform_distance_hist() {
    std::ofstream pos_out(_get_out_filename("hist_pos", "txt"));
//    std::ofstream neg_out(_get_out_filename("hist_neg", "txt"));

    bool happened = false;
    for (int i = P / 2; i < m_distance_map.rows - P / 2; i++) {
        for (int j = P / 2; j < m_distance_map.cols - P / 2; j++) {
            if (m_suspicious_zones(i, j)) {
                pos_out << m_distance_map(i, j) << " ";
                happened = true;
            }
        }
    }

    pos_out.close();

    if (!happened) {
        std::remove(_get_out_filename("hist_pos", "txt").c_str());
    }
//    neg_out.close();
}

void Detector::_perform_std_dev() {
    m_variance = compute_patch_variance(m_image, P);
    save_to_txt(_get_out_filename("variance"),
                m_variance(_get_patches_rect()));
}