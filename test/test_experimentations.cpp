#include <fstream>
#include <sstream>

#include "catch.hpp"
#include "detector.h"
#include "connected_components.h"


TEST_CASE("Test detector", "[exp]") {

    std::ifstream index("../files/in/index.txt");
    string name;
    while (std::getline(index, name)) {
        cout << name << endl;
        Detector detector(name);
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
    display_blocking(Mat_<float>(noise / 15.f));
}

TEST_CASE("NNF consistency", "[exp]") {

    std::ifstream index("../files/in/index.txt");
    string name;
    while (std::getline(index, name)) {
        cout << name << endl;
        Detector detector(name);
        detector._compute_or_load_patch_match();
        for (int i = 2; i < detector.m_image.rows - 2; i++) {
            for (int j = 2; j < detector.m_image.cols - 2; j++) {
                Vec2i p(i, j);
                auto q = detector.m_offset_map(p) + p;
                if (q[0] < 2 || q[1] < 2 || q[0] >= detector.m_image.rows - 2 || q[1] >= detector.m_image.cols - 2) {
                    FAIL("q=" << q << " is not in NNF");
                }
            }
        }

    }

}

TEST_CASE("Symmetry consistency", "[exp]") {

    std::ifstream index("../files/in/index.txt");
    string name;
    while (std::getline(index, name)) {
        cout << name << endl;
        Detector detector(name);
        detector._compute_or_load_patch_match();
        detector._perform_std_dev();
        detector._perform_symmetry_map();
        for (int i = 2; i < detector.m_image.rows - 2; i++) {
            for (int j = 2; j < detector.m_image.cols - 2; j++) {
                if (!detector.m_symmetry_map(i, j)) {
                    continue;
                }
                Vec2i p(i, j);
                auto q = detector.m_offset_map(p) + p;
                auto p_bis = detector.m_offset_map(q) + q;
                REQUIRE(detector.m_symmetry_map(q));
                REQUIRE(p_bis == p);

            }
        }

    }

}
