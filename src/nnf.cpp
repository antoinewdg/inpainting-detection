#include "nnf.h"
#include "parameters.h"

std::tuple<Mat_<Vec2i>, Mat_<int>> compute_nnf(const Mat_<Vec3i> &image) {
    typedef pm::opencv::WholeImagePatches PatchServer;
    typedef pm::opencv::OffsetMap2D OffsetMap;
    typedef pm::opencv::DistanceMap2d<int> DistanceMap;
    typedef pm::core::PatchMatcher<
            PatchServer, PatchServer, PatchDistance<PATCH_SIZE>,
            OffsetMap, DistanceMap
    > Matcher;

    OffsetMap offset_map(image.size());
    DistanceMap distance_map(image.size());
    PatchServer patches(image.size(), PATCH_SIZE);
    PatchDistance<PATCH_SIZE> patch_distance(image, MIN_PATCH_OFFSET);

    Matcher matcher(patches, patches, patch_distance, offset_map, distance_map);
    matcher.initialize_offset_map_randomly();
    matcher.iterate_n_times(5);

    // Force the symmetry of the patches after the 5
    // first iterations.
    for (Vec2i p : patches) {
        Vec2i q = p + offset_map(p);
        int d = patch_distance(q, p);
        if (d < distance_map(q)) {
            offset_map(q) = p - q;
            distance_map(q) = d;
        }
    }

    matcher.iterate_n_times(5);

    return std::make_tuple(offset_map.to_mat(), distance_map.to_mat());

};