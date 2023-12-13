#include <catch2/catch_amalgamated.hpp>

#include "../vmlib/mat44.hpp"

TEST_CASE("Vector Matrix Multiplication", "[vec4][mat44]")
{

    static constexpr float kEps_ = 1e-6f;

    using namespace Catch::Matchers;

    SECTION("Zero Vector")
    {
        Vec4f const vector = {0, 0, 0, 0};

        Mat44f const matrix = {
            {1, 1, 1, 1,
             1, 1, 1, 1,
             1, 1, 1, 1,
             1, 1, 1, 1}
        };

        Vec4f resultVector =  matrix * vector;

        REQUIRE_THAT(resultVector[0], WithinAbs(0.f, kEps_));
        REQUIRE_THAT(resultVector[1], WithinAbs(0.f, kEps_));
        REQUIRE_THAT(resultVector[2], WithinAbs(0.f, kEps_));
        REQUIRE_THAT(resultVector[3], WithinAbs(0.f, kEps_));
    }

    SECTION("Multiplication")
    {
        Vec4f const vector = {3, 5, 7, 8};

        Mat44f const matrix = {
            {2, 5, 7, 3,
             1, 2, 4, 3,
             7, 8, 9, 7,
             12, 2, 4, 3}
        };

        Vec4f resultVector =  matrix * vector;

        REQUIRE_THAT(resultVector[0], WithinAbs(104.f, kEps_));
        REQUIRE_THAT(resultVector[1], WithinAbs(65.f, kEps_));
        REQUIRE_THAT(resultVector[2], WithinAbs(180.f, kEps_));
        REQUIRE_THAT(resultVector[3], WithinAbs(98.f, kEps_));
    }

    SECTION("Scalar")
    {
        Vec4f const vector = {1, 1, 1, 1};

        Mat44f const matrix = {
            {2, 0, 0, 0,
             0, 2, 0, 0,
             0, 0, 2, 0,
             0, 0, 0, 2}
        };

        Vec4f resultVector =  matrix * vector;

        REQUIRE_THAT(resultVector[0], WithinAbs(2.f, kEps_));
        REQUIRE_THAT(resultVector[1], WithinAbs(2.f, kEps_));
        REQUIRE_THAT(resultVector[2], WithinAbs(2.f, kEps_));
        REQUIRE_THAT(resultVector[3], WithinAbs(2.f, kEps_));
    }

    SECTION("Large Values")
    {
        Vec4f const vector = {1e6f, 2e6f, 3e6f, 4e6f};

        Mat44f const matrix = {
            {12e6f, 0, 0, 0,
             0, 14e6f, 0, 0,
             0, 0, 16e6f, 0,
             0, 0, 0, 18e6f}
        };

        Vec4f resultVector =  matrix * vector;

        REQUIRE_THAT(resultVector[0], WithinAbs(12e12f, kEps_));
        REQUIRE_THAT(resultVector[1], WithinAbs(28e12f, kEps_));
        REQUIRE_THAT(resultVector[2], WithinAbs(48e12f, kEps_));
        REQUIRE_THAT(resultVector[3], WithinAbs(72e12f, kEps_));
    }
}