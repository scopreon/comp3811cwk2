#include <catch2/catch_amalgamated.hpp>

#include "../vmlib/mat44.hpp"

TEST_CASE("Matrix Projection", "[projection][mat44]")
{

    static constexpr float kEps_ = 1e-6f;

    using namespace Catch::Matchers;

    SECTION("Standard")
    {
        Mat44f matrix = make_perspective_projection(20, 4, 2, 7);

        REQUIRE_THAT(matrix(0,0), WithinAbs(0.3855877, kEps_));
        REQUIRE_THAT(matrix(0,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(matrix(0,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(matrix(0,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(matrix(1,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(matrix(1,1), WithinAbs(1.54235f, kEps_));
        REQUIRE_THAT(matrix(1,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(matrix(1,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(matrix(2,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(matrix(2,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(matrix(2,2), WithinAbs(-1.8, kEps_));
        REQUIRE_THAT(matrix(2,3), WithinAbs(-5.6f, kEps_));

        REQUIRE_THAT(matrix(3,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(matrix(3,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(matrix(3,2), WithinAbs(-1.f, kEps_));
        REQUIRE_THAT(matrix(3,3), WithinAbs(0.f, kEps_));
    }

    SECTION("Negatives")
    {
        Mat44f matrix = make_perspective_projection(-10, -5, -12, -2);

        REQUIRE_THAT(matrix(0,0), WithinAbs(-0.0591629, kEps_));
        REQUIRE_THAT(matrix(0,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(matrix(0,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(matrix(0,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(matrix(1,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(matrix(1,1), WithinAbs(0.295813, kEps_));
        REQUIRE_THAT(matrix(1,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(matrix(1,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(matrix(2,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(matrix(2,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(matrix(2,2), WithinAbs(1.4, kEps_));
        REQUIRE_THAT(matrix(2,3), WithinAbs(-4.8f, kEps_));

        REQUIRE_THAT(matrix(3,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(matrix(3,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(matrix(3,2), WithinAbs(-1.f, kEps_));
        REQUIRE_THAT(matrix(3,3), WithinAbs(0.f, kEps_));
    }
}