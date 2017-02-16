#include "inpainting_detector.h"

InpaintingDetector::InpaintingDetector(DiskCache &disk_cache) : m_disk_cache(disk_cache) {
    m_image = m_disk_cache.get_input_image();
    auto nnf_computation = [&]() { return compute_nnf(m_image); };
    std::tie(m_offset_map, m_distance_map) = m_disk_cache.get_nnf(nnf_computation);
}

vector<std::pair<float, Vec2i>> InpaintingDetector::get_dominant_offsets() {
    Mat_<float> counts(2 * m_image.rows, 2 * m_image.cols, 0.f);

    for_each_patch<P>(m_offset_map, [&](int i, int j) {
        Vec2i o = m_offset_map(i, j);
        counts(o[0] + m_image.rows, o[1] + m_image.cols)++;
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