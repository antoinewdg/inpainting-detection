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


TEST_CASE("Noise estimation", "[exp]") {
    std::ofstream out("../files/out/noise_levels.txt");
    vector<string> stdevs = {"2.5", "5.0", "7.5", "10.0", "12.5", "15.0", "17.5", "20.0", "22.5"};
    for (string stdev : stdevs) {
        Mat_<Vec3b> image = load_color("../files/noise/noise_" + stdev + ".png");
        float noise = estimate_noise_level(image);
        cout << stdev << ": " << noise << endl;
        out << noise << " ";
    }
    out.close();
}