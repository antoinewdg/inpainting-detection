#include "catch.hpp"
#include "connected_components.h"

#include "utils.h"
#include "test_utils.h"

TEST_CASE("Connected components") {
    auto image = load_test_asset("connected_components/image.png");
    auto expected_labels = load_test_asset("connected_components/labels.png");

    ConnectedComponentsFinder<Vec3b> finder(image);
    auto labels = finder.get_connected_components();

    SECTION("get_connected_components") {

        /*
         * For each pixel of the image, test that, for each other pixel
         * either the label is the same both in the expected labels and in *
         * the actual labels, or is different in both.
         */
        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                int label = labels(i, j);
                Vec3b expected_label = expected_labels(i, j);
                for (int k = 0; k < image.rows; k++) {
                    for (int l = 0; l < image.rows; l++) {
                        if (labels(k, l) == label ^ expected_labels(k, l) == expected_label) {
                            FAIL(i << " " << j << " - " << k << " " << l);
                        }
                    }
                }
            }
        }
    }

    SECTION("get_areas_as_vector"){
        vector<int> areas = finder.get_areas_as_vector();
        REQUIRE(areas[labels(0,0)] == 137);
        REQUIRE(areas[labels(0,12)] == 21);
        REQUIRE(areas[labels(6,14)] == 18);
        REQUIRE(areas[labels(10,10)] == 27);
        REQUIRE(areas[labels(14,0)] == 14);
        REQUIRE(areas[labels(14,7)] == 38);
        REQUIRE(areas[labels(15,13)] == 1);
    }


    REQUIRE(true);
}