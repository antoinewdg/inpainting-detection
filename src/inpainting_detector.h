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

    /**
     * Identify dominant offsets in the nnf.
     * @return
     */
    vector<std::pair<float, Vec2i>> get_dominant_offsets();

    /**
     * Compute one-way offset relevance.
     * @param o offset
     * @param alpha
     * @return
     */
    Mat_<bool> get_offset_relevance(Vec2i o, float alpha);

    /**
     * Compute the symmetric offset relevance.
     * @param o offset
     * @param alpha
     * @return
     */
    Mat_<bool> get_symmetric_offset_relevance(Vec2i o, float alpha);

    /**
     * Identify patches having a high enough variance.
     */
    void compute_non_flat_patches();

    /**
     * Compute the global detection mask.
     *
     * @return
     */
    Mat_<bool> get_detection_mask();

    /**
     * Validate a dominant offset.
     *
     * @param o offset
     * @param detection_mask
     * @param k not used, useful if one wants to write files here
     */
    void validate_dominant_offset(Vec2i o, Mat_<bool> &detection_mask, int k);

private:
    DiskCache &m_disk_cache;

    Mat_<Vec3b> m_image;
    Mat_<Vec2i> m_offset_map;
    Mat_<int> m_distance_map;
    Mat_<bool> m_non_flat_patches;
};

#endif //INPAINTING_DECTECTION_INPAINTING_DETECTOR_H
