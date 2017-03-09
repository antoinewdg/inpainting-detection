#include "inpainting_detector.h"

InpaintingDetector::InpaintingDetector(DiskCache &disk_cache) : m_disk_cache(disk_cache) {
    m_image = m_disk_cache.get_input_image();
    auto nnf_computation = [&]() { return compute_nnf(m_image); };
    std::tie(m_offset_map, m_distance_map) = m_disk_cache.get_nnf(nnf_computation);
    compute_non_flat_patches();
}


void InpaintingDetector::compute_non_flat_patches() {
    m_non_flat_patches = Mat_<bool>(m_image.size(), false);
    Mat_<float> variance = compute_patch_variance(m_image, P);
    m_non_flat_patches = variance >= VARIANCE_THRESHOLD;
}



vector<std::pair<float, Vec2i>> InpaintingDetector::get_dominant_offsets() {
    Mat_<float> counts(2 * m_image.rows, 2 * m_image.cols, 0.f);

    for_each_patch<P>(m_offset_map, [&](int i, int j) {

        Vec2i o = m_offset_map(i, j);
        if (m_non_flat_patches(i, j)) {
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

    // Compute the needed boundaries.
    int i_min = std::max(2, 2 - o[0]);
    int i_max = std::min(m_image.rows - 2 - o[0], m_image.rows - 2);
    int j_min = std::max(2, 2 - o[1]);
    int j_max = std::min(m_image.cols - 2 - o[1], m_image.cols - 2);

    for (int i = i_min; i < i_max; i++) {
        for (int j = j_min; j < j_max; j++) {
            Vec2i p(i, j);
            Vec2i q = p + o;
            float d = patch_distance(p, q, int(alpha * m_distance_map(p) + 1));
            relevance(i, j) = (d <= alpha * m_distance_map(i, j)) && m_non_flat_patches(i, j);
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




void InpaintingDetector::validate_dominant_offset(Vec2i o, Mat_<bool> &detection_mask, int k) {
        double norm = std::sqrt(o[0] * o[0] + o[1] * o[1]);
    if (norm <= RELEVANT_OFFSET_NORM) {
        return;
    }

    Mat_<bool> relevance = get_symmetric_offset_relevance(o, ALPHA);

    ConnectedComponentsFinder<bool> finder(relevance);
    auto labels = finder.get_connected_components();
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

    for_each_patch<P>(m_image, [&](int i, int j) {
        bool is_label_relevant = relevant_labels.find(labels(i, j)) != relevant_labels.end();
        detection_mask(i, j) = detection_mask(i, j) | is_label_relevant;
    });


}

Mat_<bool> InpaintingDetector::get_detection_mask() {
    Mat_<bool> detection_mask(m_image.size(), false);
    auto offsets = get_dominant_offsets();
    offsets.resize(N_DOMINANT);

    int k = 0;
    for (auto p : offsets) {
        validate_dominant_offset(p.second, detection_mask, k);
        k++;
    }

    string filename = m_disk_cache.get_out_filename("detection_mask", "png");
    cv::imwrite(filename, 255 * detection_mask);

    return detection_mask;
}