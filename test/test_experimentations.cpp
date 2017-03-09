#include <fstream>
#include <sstream>
#include <thread>

#include "catch.hpp"
#include "inpainting_detector.h"
#include "connected_components.h"
#include "parameters.h"

TEST_CASE("Test detector", "[exp]") {

    std::ifstream index("../files/in/index.txt");
    string name;

    std::ofstream out("../files/out/scores.txt");
    while (std::getline(index, name)) {
        cout << name << endl;
        out << name;
        DiskCache cache(name);
        InpaintingDetector detector(cache);
        detector.get_detection_mask();
    }

    out.close();

}

void call_from_thread(int tid, vector<string> names) {

    for (string name : names) {
        cout << name << " -- " << tid << endl;
        DiskCache cache(name);
        InpaintingDetector detector(cache);
        detector.get_detection_mask();

        cout << endl;
    }
}

TEST_CASE("Parallel", "[exp]") {
    const int N = 8;
    std::thread t[N];

    vector<vector<string>> names(N);
    std::ifstream index("../files/in/index.txt");

    string name;
    int l = 0;
    while (std::getline(index, name)) {
        names[l % N].push_back(name);
        l++;
    }

    for (int i = 0; i < N; i++) {
        t[i] = std::thread(call_from_thread, i, names[i]);
    }

    for (int i = 0; i < N; i++) {
        t[i].join();
    }
}

