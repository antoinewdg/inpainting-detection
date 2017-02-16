#include "inpainting_detector.h"

InpaintingDetector::InpaintingDetector(DiskCache &disk_cache) : m_disk_cache(disk_cache) {
    m_image = m_disk_cache.get_input_image();
    auto nnf_computation = [&]() { return compute_nnf(m_image); };
    std::tie(m_offset_map, m_distance_map) = m_disk_cache.get_nnf(nnf_computation);

    display_blocking(m_image);
}