#include "catch.hpp"
#include "detector.h"
#include "connected_components.h"


TEST_CASE("Test detector") {
//    auto image = load_color("../files/beach_original.jpg");
//    display_and_block(image);
    vector<string> names = {
//            "beach_inpainted", "beach_original",
//            "brick_wall_inpainted", "brick_wall_original",
//            "bridge_lamp_inpainted", "bridge_lamp_original",
            "temple_inpainted", "temple_original",
//            "ny_inpainted", "ny_original"
    };

    for (string name : names) {
        Detector detector(name);
        cout << name << endl;
        Mat_<Vec2i> offset_map = detector.build_offset_map();
        ConnectedComponentsFinder<Vec2i> finder(offset_map);
        Mat_<int> labels = finder.get_connected_components();
//        std::tie(labels, areas) = label_connected_components(offset_map);
        display_labels(labels);
    }


}