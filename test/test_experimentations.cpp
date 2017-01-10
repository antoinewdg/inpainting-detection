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