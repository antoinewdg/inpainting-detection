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

TEST_CASE("Local noise estimation", "[unit]") {
    Mat_<Vec3b> im = load_test_asset("misc/local_noises.png");
    Mat_<float> n = estimate_local_noise_level(im, 20);

    REQUIRE(n(25, 25) == Approx(10).epsilon(0.5));
    REQUIRE(n(25, 75) == Approx(7.5).epsilon(0.5));
    REQUIRE(n(75, 25) == Approx(5).epsilon(0.5));
    REQUIRE(n(75, 75) == Approx(2.5).epsilon(0.5));

}