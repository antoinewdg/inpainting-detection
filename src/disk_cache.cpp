#include "disk_cache.h"

Mat_<Vec3b> DiskCache::get_input_image() {
    return load_color("../files/in/" + m_name);
}

string DiskCache::get_out_filename(string dir, string ext, string suffix) {
    namespace fs = boost::filesystem;
    string path = "../files/out/" + dir + "/" + m_name + suffix + "." + ext;
    fs::create_directories(fs::path(path).parent_path());
    return path;
}
