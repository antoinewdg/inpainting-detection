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