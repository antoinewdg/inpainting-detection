#include "inpainting_detector.h"

InpaintingDetector::InpaintingDetector(DiskCache &disk_cache) : m_disk_cache(disk_cache) {
    m_image = m_disk_cache.get_input_image();
    auto nnf_computation = [&]() { return compute_nnf(m_image); };
    std::tie(m_offset_map, m_distance_map) = m_disk_cache.get_nnf(nnf_computation);
    compute_valid_patches();
    compute_symmetry_map();
}


void InpaintingDetector::compute_valid_patches() {
    m_valid_patches = Mat_<bool>(m_image.size(), false);
    Mat_<float> variance = compute_patch_variance(m_image, P);
    m_valid_patches = variance >= VARIANCE_THRESHOLD;
}

void InpaintingDetector::compute_symmetry_map() {
    m_symmetry_map = Mat_<bool>(m_image.size(), false);
    for_each_patch<P>(m_offset_map, [&](int i, int j) {
        Vec2i p(i, j);
        Vec2i q = m_offset_map(p) + p;
        Vec2i p_bis = m_offset_map(q) + q;
        if (p_bis == p) {
            m_symmetry_map(p) = true;
        }
    });
}


vector<std::pair<float, Vec2i>> InpaintingDetector::get_dominant_offsets() {
    Mat_<float> counts(2 * m_image.rows, 2 * m_image.cols, 0.f);

    for_each_patch<P>(m_offset_map, [&](int i, int j) {

        Vec2i o = m_offset_map(i, j);
        if (m_valid_patches(i, j)) {//} && m_symmetry_map(i, j)) {
            counts(o[0] + m_image.rows, o[1] + m_image.cols)++;
        }
    });

    vector<std::pair<float, Vec2i>> result;

    for_each_pixel(counts, [&](int i, int j, float &c) {
        if (c > 0) {
            result.emplace_back(c, Vec2i(i - m_image.rows, j - m_image.cols));
        }
    });

    auto comp = [](const std::pair<float, Vec2i> &p, const std::pair<float, Vec2i> &q) {
        return p.first > q.first;
    };
    std::sort(result.begin(), result.end(), comp);

    return result;
}

Mat_<bool> InpaintingDetector::get_offset_relevance(Vec2i o, float alpha) {
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
            float d = patch_distance(p, q, int(alpha * m_distance_map(p) + 1));
            relevance(i, j) = (d <= alpha * m_distance_map(i, j));//&& m_valid_patches(i, j);
        }
    }

    return relevance;
}

Mat_<bool> InpaintingDetector::get_symmetric_offset_relevance(Vec2i o, float alpha) {

    Mat_<bool> relevance = get_offset_relevance(o, alpha);
    Mat_<bool> reverse = get_offset_relevance(-o, alpha);

    for_each_pixel(relevance, [&](int i, int j, bool &b) {
        b = b && reverse(Vec2i(i, j) + o);
    });

    return relevance;
}

float norm_penalization(Vec2i o) {
    float norm = std::sqrt(o[0] * o[0] + o[1] * o[1]);
    if (norm >= RELEVANT_OFFSET_NORM) {
        return 1.f;
    }

    return (norm - MIN_PATCH_OFFSET) / (RELEVANT_OFFSET_NORM - MIN_PATCH_OFFSET);

}

float penalization(Vec2i o) {
    return norm_penalization(o);
}

Mat_<bool> InpaintingDetector::get_detection_mask(Vec2i o) {
    Mat_<bool> relevance = get_symmetric_offset_relevance(o, 0);

    Mat_<float> rel = relevance / 255;
    Mat_<float> kernel(5, 5, 1.f / (5 * 5));
    cv::filter2D(rel, rel, CV_32F, kernel);
    rel = rel > 0.6f;

    ConnectedComponentsFinder<float> finder(rel);
    auto cc = finder.get_connected_components();
    vector<int> areas = finder.get_areas_as_vector_without_bg(0.f);
    auto max_it = std::max_element(areas.begin(), areas.end());
    cout << penalization(o) * (*max_it) << endl;

    display_blocking(relevance);

    return rel;
}


void InpaintingDetector::validate_dominant_offset(Vec2i o, Mat_<bool> &detection_mask, int k) {
    double norm = std::sqrt(o[0] * o[0] + o[1] * o[1]);
    if (norm <= RELEVANT_OFFSET_NORM) {
        return;
    }

    Mat_<bool> relevance = get_symmetric_offset_relevance(o, ALPHA);

    ConnectedComponentsFinder<bool> finder(relevance);
    auto cc = finder.get_connected_components();
    vector<int> areas = finder.get_areas_as_vector_without_bg(false);
    std::unordered_set<int> relevant_labels;

    int count_pos = 0, count_neg = 0;
    bool found_huge = false;
    for (int l = 0; l < areas.size(); l++) {
        int area = areas[l];
        if (area > A_HUGE) {
            found_huge = true;
            relevant_labels.insert(l);
        }
        if (area > A_MIN) {
            count_pos += area;
            relevant_labels.insert(l);
        } else if (area > 5) {
            count_neg += area;
        }
    }

    float ratio = float(count_pos) / (count_neg + 1);
    if (!found_huge && ratio < R_MIN) {
        return;
    }

    Mat_<bool> temp(m_image.size(), false);
    for_each_patch<P>(m_image, [&](int i, int j) {
        bool is_label_relevant = relevant_labels.find(cc(i, j)) != relevant_labels.end();
        detection_mask(i, j) = detection_mask(i, j) | is_label_relevant;
        temp(i, j) = is_label_relevant;
    });


    string n = std::to_string(k);
    while (n.size() < 3) {
        n = "0" + n;
    }
    string filename = m_disk_cache.get_out_filename("offset_relevance", "png", "_" + n);
    cv::imwrite(filename, 255 * relevance);

    filename = m_disk_cache.get_out_filename("offset_presence", "png", "_" + n);
    cv::imwrite(filename, 255 * get_symmetric_offset_relevance(o, 1.f));
}

Mat_<bool> InpaintingDetector::get_detection_mask() {
    Mat_<bool> detection_mask(m_image.size(), false);
    auto offsets = get_dominant_offsets();
    offsets.resize(N_DOMINANT);

    int k = 0;
    for (auto p : offsets) {
        if (p.first < A_MIN) { break; }
        validate_dominant_offset(p.second, detection_mask, k);
        k++;
    }

    string filename = m_disk_cache.get_out_filename("detection_mask", "png", "_m");
    cv::imwrite(filename, 255 * detection_mask);

    filename = m_disk_cache.get_out_filename("detection_mask", "png", "_o");
    cv::imwrite(filename, m_image);

    return detection_mask;
}