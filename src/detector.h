//
// Created by antoinewdg on 11/27/16.
//

#ifndef INPAINTING_DECTECTION_DECTECTOR_H
#define INPAINTING_DECTECTION_DECTECTOR_H

#include <boost/filesystem.hpp>

#include "utils.h"
#include "misc.h"
#include "patch_distance.h"
#include "patch_match_core/patch_matcher.h"
#include "patch_match_opencv/adapters.h"

#include "connected_components.h"

class Detector {

public:
    static constexpr int P = 5;
    static constexpr float STD_DEV_THRESHOLD = 7.0;
    typedef pm::opencv::WholeImagePatches PatchServer;
    typedef pm::opencv::OffsetMap2D OffsetMap;
    typedef pm::opencv::DistanceMap2d<int> DistanceMap;
    typedef pm::core::PatchMatcher<
            PatchServer, PatchServer, PatchDistance<P>,
            OffsetMap, DistanceMap
    > Matcher;

    Detector(string name) : m_name(name) {
        m_image = load_color("../files/in/" + name);
    }


    void perform_operations() {
//        ConnectedComponentsFinder<Vec2i> finder(m_offset_map);
//        m_connected_components = finder.get_connected_components();
        _compute_or_load_patch_match();
        _perform_std_dev();
        _perform_is_mirror();
        _perform_suspicious_zones();

    }

    inline string _get_out_filename(string dir, string ext = "png", string suffix = "") {
        namespace fs = boost::filesystem;
        string path = "../files/out/" + dir + "/" + m_name + suffix + "." + ext;
        fs::create_directories(fs::path(path).parent_path());
        return path;
    }

    inline Rect _get_patches_rect() {
        return Rect(P / 2, P / 2, m_image.cols - P + 1, m_image.rows - P + 1);
    }


    bool _file_exists(string filename) {
        cv::FileStorage fs(filename, cv::FileStorage::READ);
        bool exists = fs.isOpened();
        fs.release();
        return exists;
    }

    void _perform_patch_match();

    void _compute_or_load_patch_match();

    void _perform_is_mirror();

    void _perform_suspicious_zones();

    void _perform_std_dev();

    Mat_<Vec3b> m_image;
    string m_name;
    Mat_<Vec2i> m_offset_map;
    Mat_<int> m_distance_map;
    Mat_<int> m_connected_components;
    Mat_<uchar> m_is_mirror;
    Mat_<float> m_std_dev;
};

#endif //INPAINTING_DECTECTION_DECTECTOR_H
