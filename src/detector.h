//
// Created by antoinewdg on 11/27/16.
//

#ifndef INPAINTING_DECTECTION_DECTECTOR_H
#define INPAINTING_DECTECTION_DECTECTOR_H


#include "utils.h"
#include "patch_distance.h"
#include "patch_match_core/patch_matcher.h"
#include "patch_match_opencv/adapters.h"

class Detector {

public:
    static constexpr int P = 5;
    typedef pm::opencv::WholeImagePatches PatchServer;
    typedef pm::opencv::OffsetMap2D OffsetMap;
    typedef pm::opencv::DistanceMap2d<unsigned int> DistanceMap;
    typedef pm::core::PatchMatcher<
            PatchServer, PatchServer, PatchDistance<P>,
            OffsetMap, DistanceMap
    > Matcher;

    Detector(string name) : m_name(name) {
        m_image = load_color("../files/" + name + ".png");
    }

    Mat_<Vec2i> build_offset_map() {

        if (_offsets_file_exists()) {
            return load_from_yaml<Vec2i>(_offsets_filename());
        }
        OffsetMap offset_map(m_image.size());
        DistanceMap distance_map(m_image.size());
        PatchServer patches(m_image.size(), P);
        PatchDistance<P> patch_distance(m_image, 20);

        Matcher matcher(patches, patches, patch_distance, offset_map, distance_map);
        matcher.initialize_offset_map_randomly();
        matcher.iterate_n_times(5);

        write_to_yaml(_offsets_filename(), offset_map.to_mat());


        return offset_map.to_mat();

    }

private:

    string _offsets_filename() {
        return "../files/offsets/" + m_name + ".yml";
    }

    bool _offsets_file_exists() {
        cv::FileStorage fs(_offsets_filename(), cv::FileStorage::READ);
        bool exists = fs.isOpened();
        fs.release();
        return exists;
    }

    Mat_<Vec3b> m_image;
    string m_name;
};

#endif //INPAINTING_DECTECTION_DECTECTOR_H
