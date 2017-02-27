//
// Created by antoinewdg on 2/16/17.
//

#ifndef INPAINTING_DECTECTION_DISK_CACHE_H
#define INPAINTING_DECTECTION_DISK_CACHE_H

#include <boost/filesystem.hpp>

#include "utils.h"
#include "nnf.h"
#include "parameters.h"

class DiskCache {
public:
    DiskCache(string name) : m_name(name) {}

    Mat_<Vec3b> get_input_image();

    template<class NNFComputation>
    std::tuple<Mat_<Vec2i>, Mat_<int>> get_nnf(NNFComputation nnf_computation) {
        string offsets_name = get_out_filename("offsets", "yml");
        string distances_name = get_out_filename("distances", "yml");

        if (_file_exists(offsets_name) && _file_exists(distances_name)) {
            auto offset_map = load_from_yaml<Vec2i>(offsets_name);
            auto distance_map = load_from_yaml<int>(distances_name);
            return std::make_tuple(offset_map, distance_map);
        }

        auto result = nnf_computation();

        if (SAVE_PATCH_MATCH) {
            write_to_yaml(offsets_name, std::get<0>(result));
            write_to_yaml(distances_name, std::get<1>(result));
        }

        return result;
    };

    string get_out_filename(string dir, string ext = "png", string suffix = "");

private:

    bool _file_exists(string filename) {
        cv::FileStorage fs(filename, cv::FileStorage::READ);
        bool exists = fs.isOpened();
        fs.release();
        return exists;
    }

    string m_name;
};

#endif //INPAINTING_DECTECTION_DISK_CACHE_H
