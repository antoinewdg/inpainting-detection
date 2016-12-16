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

void Detector::_perform_is_mirror() {
    m_is_mirror = Mat_<uchar>(m_offset_map.size(), 0);
    for (int i = 2; i < m_offset_map.rows - 2; i++) {
        for (int j = 2; j < m_offset_map.cols - 2; j++) {
            Vec2i p(i, j);
            Vec2i q = m_offset_map(p) + p;
            Vec2i p_bis = m_offset_map(q) + q;
            m_is_mirror(p) = uchar(255) * uchar(p == p_bis);
        }
    }

    Rect r(2, 2, m_is_mirror.cols - 4, m_is_mirror.rows - 4);
    m_is_mirror = m_is_mirror(r);

    string mirror_name = _get_out_filename("is_mirror", "png", std::to_string(0));
    cv::imwrite(mirror_name, m_is_mirror);

    std::ofstream out(_get_out_filename("granulometry", "txt"));

    int M = cv::countNonZero(m_is_mirror);

    for (int n = 1; n < 10; n++) {
        Mat_<uchar> opening;
        auto struct_el = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * n + 1, 2 * n + 1));
        cv::morphologyEx(m_is_mirror, opening, cv::MORPH_OPEN, struct_el);
        float c = float(cv::countNonZero(opening)) / M;
        out << " " << c;
        if (c != 0) {
            string file_name = _get_out_filename("is_mirror", "png", std::to_string(n));
            cv::imwrite(file_name, opening);
        }
    }

    out.close();
}

void Detector::_perform_suspicious_zones() {
    Mat_<Vec3b> image(m_image.size());
    for (int i = 2; i < image.rows - 2; i++) {
        for (int j = 2; j < image.cols - 2; j++) {
            image(i, j) = m_image(i, j);
            if (!m_is_mirror(i, j)) {
                image(i, j) = image(i, j) - Vec3b(70, 70, 70);
            }
        }
    }
    string name = _get_out_filename("is_mirror", "png", "_99");
    cv::imwrite(name, image);
}

void Detector::_perform_variance() {
    m_variance = compute_patch_variance(m_image, P);
    save_to_txt(_get_out_filename("variance"),
                m_variance(_get_patches_rect()));
}