#include <fstream>
#include <sstream>
#include <thread>

#include "catch.hpp"
#include "inpainting_detector.h"
#include "connected_components.h"


TEST_CASE("Test detector", "[exp]") {

    std::ifstream index("../files/in/index.txt");
    string name;

    std::ofstream out("../files/out/scores.txt");
    while (std::getline(index, name)) {
        cout << name << endl;
        out << name;
        DiskCache cache(name);
        InpaintingDetector detector(cache);
        auto dominant = detector.get_dominant_offsets();
        dominant.resize(1);
        for (auto p : dominant) {
            float score = detector.perform_detection(p.second);
            out << " " << score;
        }
        out << endl;
    }

    out.close();

}

void call_from_thread(int tid, vector<string> names) {

//    cout << "Launching thread" << tid << endl;
    std::ofstream out("../files/out/scores_" + std::to_string(tid) + ".txt");
    for (string name : names) {
        cout << name << " -- " << tid << endl;
        DiskCache cache(name);
        InpaintingDetector detector(cache);
        auto dominant = detector.get_dominant_offsets();
        dominant.resize(1);
        for (auto p : dominant) {
            float score = detector.perform_detection(p.second);
            out << name << " " << score;
        }
        out << endl;
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
