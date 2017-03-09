#include <fstream>
#include <sstream>
#include <thread>

#include "inpainting_detector.h"

void call_from_thread(int tid, vector<string> names) {

    for (string name : names) {
        cout << name << " -- " << tid << endl;
        DiskCache cache(name);
        InpaintingDetector detector(cache);
        detector.get_detection_mask();
    }
}

/**
 * Main function of the program.
 *
 * Loops through all the file names in index.txt,
 * divides the workload for each processor and
 * performs the detections in parallel.
 * To choose the number of threads spawned, edit the
 * N variable.
 *
 * @return
 */
int main () {
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
    return 0;
}