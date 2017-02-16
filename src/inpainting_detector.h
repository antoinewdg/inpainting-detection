//
// Created by antoinewdg on 2/16/17.
//

#ifndef INPAINTING_DECTECTION_INPAINTING_DETECTOR_H
#define INPAINTING_DECTECTION_INPAINTING_DETECTOR_H

#include "utils.h"
#include "disk_cache.h"
#include "nnf.h"

class InpaintingDetector {
public:
    InpaintingDetector(DiskCache &disk_cache);

private:
    DiskCache &m_disk_cache;

    Mat_<Vec3b> m_image;
    Mat_<Vec2i> m_offset_map;
    Mat_<int> m_distance_map;
};

#endif //INPAINTING_DECTECTION_INPAINTING_DETECTOR_H
