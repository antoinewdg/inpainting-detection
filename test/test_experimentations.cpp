#include <fstream>
#include <sstream>

#include "catch.hpp"
#include "detector.h"
#include "connected_components.h"


TEST_CASE("Test detector", "[exp]") {

    std::ifstream index("../files/in/index.txt");
    string name;
    while (std::getline(index, name)) {

        Detector detector(name);
        float noise = estimate_noise_level(detector.m_image);

        cout << name << " " << noise << endl;
        detector.perform_operations();
    }

}

TEST_CASE("Visualize local noise", "[exp]") {
    Mat_<Vec3b> image = load_color("../files/in/misc/beach.png");
    Mat_<float> noise = estimate_local_noise_level(image, 20);

    std::ofstream out("../files/out/noise_hist.txt");
    out << estimate_noise_level(image) << " ";
    for (float f : noise) {
        out << f << " ";
    }
    out.close();
    display_and_block(Mat_<float>(noise / 15.f));
}