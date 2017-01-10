#include "catch.hpp"

#include "misc.h"
#include "test_utils.h"

TEST_CASE("Hysteresis", "[unit]") {
    Mat_<bool> in_high = load_mask_asset("misc/histeresis_high.png");
    Mat_<bool> in_low = load_mask_asset("misc/histeresis_low.png");
    Mat_<bool> expected_out = load_mask_asset("misc/histeresis_out.png");

    Mat_<bool> actual_out = hysteresis_filter(in_high, in_low);
    require_matrix_equal(expected_out, actual_out);
}

TEST_CASE("Noise estimation", "[unit]") {
    vector<string> stdevs = {"2.5", "5.0", "7.5", "10.0", "12.5", "15.0", "17.5", "20.0", "22.5"};
    for (string stdev : stdevs) {
        float v = std::stof(stdev);
        Mat_<Vec3b> noise = load_test_asset("misc/noise_" + stdev + ".png");
        float actual = estimate_noise_level(noise);
        float err = std::abs(actual - v) / v;
        REQUIRE(err < 0.05);
    }
}