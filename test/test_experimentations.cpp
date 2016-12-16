#include <fstream>
#include <sstream>

#include "catch.hpp"
#include "detector.h"
#include "connected_components.h"


TEST_CASE("Test detector") {

    std::ifstream index("../files/in/index.txt");
    string name;
    while (std::getline(index, name)) {
        cout << name << endl;
        Detector detector(name);
        detector.perform_operations();
    }


}