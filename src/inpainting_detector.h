//
// Created by antoinewdg on 2/16/17.
//

#ifndef INPAINTING_DECTECTION_INPAINTING_DETECTOR_H
#define INPAINTING_DECTECTION_INPAINTING_DETECTOR_H

#include <unordered_set>

#include "utils.h"
#include "disk_cache.h"
#include "nnf.h"
#include "misc.h"
#include "connected_components.h"
#include "parameters.h"

class InpaintingDetector {
    static constexpr int P = PATCH_SIZE;
public:
    InpaintingDetector(DiskCache &disk_cache);

    vector<std::pair<float, Vec2i>> get_dominant_offsets();

    Mat_<bool> get_offset_relevance(Vec2i o, float alpha);

    Mat_<bool> get_symmetric_offset_relevance(Vec2i o, float alpha);

    void compute_valid_patches();

    void compute_symmetry_map();

    Mat_<bool> get_detection_mask();

    void validate_dominant_offset(Vec2i o, Mat_<bool> &detection_mask, int k);

private:
    DiskCache &m_disk_cache;

    Mat_<Vec3b> m_image;
    Mat_<Vec2i> m_offset_map;
    Mat_<int> m_distance_map;
    Mat_<bool> m_valid_patches, m_symmetry_map;
};

#endif //INPAINTING_DECTECTION_INPAINTING_DETECTOR_H
