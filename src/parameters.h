//
// Created by antoinewdg on 2/21/17.
//

#ifndef INPAINTING_DECTECTION_PARAMETERS_H
#define INPAINTING_DECTECTION_PARAMETERS_H

extern const bool SAVE_PATCH_MATCH; // if true the patchmatch results will be cached

const int PATCH_SIZE = 5;
extern const float A_MIN;
extern const float A_HUGE;
extern const float ALPHA;
extern const float R_MIN;
extern const int N_DOMINANT;
extern const int MIN_PATCH_OFFSET;
extern const int RELEVANT_OFFSET_NORM;
extern const float VARIANCE_THRESHOLD;

#endif //INPAINTING_DECTECTION_PARAMETERS_H
