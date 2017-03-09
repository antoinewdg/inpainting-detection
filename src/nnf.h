//
// Created by antoinewdg on 2/16/17.
//

#ifndef INPAINTING_DECTECTION_NNF_H
#define INPAINTING_DECTECTION_NNF_H

#include <tuple>

#include "patch_distance.h"
#include "patch_match_core/patch_matcher.h"
#include "patch_match_opencv/adapters.h"

#include "utils.h"

/**
 * Compute the NNF of an image on itself.
 *
 * @param image
 * @return the offset field and the distance map
 */
std::tuple<Mat_<Vec2i>, Mat_<int>> compute_nnf(const Mat_<Vec3i> &image);

#endif //INPAINTING_DECTECTION_NNF_H
