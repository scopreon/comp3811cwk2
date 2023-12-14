#include <catch2/catch_amalgamated.hpp>

#include "../vmlib/mat44.hpp"

TEST_CASE("Matrix Multiplication", "[multiplication][mat44]")
{

    static constexpr float kEps_ = 1e-6f;

    using namespace Catch::Matchers;

    SECTION("Multiply")
    {

        Mat44f const matrixA = {
            {3, 5, 1, 9,
             7, 5, 12, 4,
             8, 8, 2, 3,
             4, 2, 11, 14}
        };
        Mat44f const matrixB = {
            {5, 8, 7, 4,
             13, 2, 4, 3,
             5, 13, 6, 2,
             8, 4, 3, 10}
        };

        Mat44f const rMatrix = matrixA * matrixB;

        REQUIRE_THAT(rMatrix(0,0), WithinAbs(157.f, kEps_));
        REQUIRE_THAT(rMatrix(0,1), WithinAbs(83.f, kEps_));
        REQUIRE_THAT(rMatrix(0,2), WithinAbs(74.f, kEps_));
        REQUIRE_THAT(rMatrix(0,3), WithinAbs(119.f, kEps_));

        REQUIRE_THAT(rMatrix(1,0), WithinAbs(192.f, kEps_));
        REQUIRE_THAT(rMatrix(1,1), WithinAbs(238.f, kEps_));
        REQUIRE_THAT(rMatrix(1,2), WithinAbs(153.f, kEps_));
        REQUIRE_THAT(rMatrix(1,3), WithinAbs(107.f, kEps_));

        REQUIRE_THAT(rMatrix(2,0), WithinAbs(178.f, kEps_));
        REQUIRE_THAT(rMatrix(2,1), WithinAbs(118.f, kEps_));
        REQUIRE_THAT(rMatrix(2,2), WithinAbs(109.f, kEps_));
        REQUIRE_THAT(rMatrix(2,3), WithinAbs(90.f, kEps_));

        REQUIRE_THAT(rMatrix(3,0), WithinAbs(213.f, kEps_));
        REQUIRE_THAT(rMatrix(3,1), WithinAbs(235.f, kEps_));
        REQUIRE_THAT(rMatrix(3,2), WithinAbs(144.f, kEps_));
        REQUIRE_THAT(rMatrix(3,3), WithinAbs(184.f, kEps_));
    }

    SECTION("Double")
    {

        Mat44f const matrixA = {
            {2, 0, 0, 0,
             0, 2, 0, 0,
             0, 0, 2, 0,
             0, 0, 0, 2}
        };
        Mat44f const matrixB = {
            {1, 1, 1, 1,
             1, 1, 1, 1,
             1, 1, 1, 1,
             1, 1, 1, 1}
        };

        Mat44f const rMatrix = matrixA * matrixB;

        REQUIRE_THAT(rMatrix(0,0), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(rMatrix(0,1), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(rMatrix(0,2), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(rMatrix(0,3), WithinAbs(2.f, kEps_));

        REQUIRE_THAT(rMatrix(1,0), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(rMatrix(1,1), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(rMatrix(1,2), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(rMatrix(1,3), WithinAbs(2.f, kEps_));

        REQUIRE_THAT(rMatrix(2,0), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(rMatrix(2,1), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(rMatrix(2,2), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(rMatrix(2,3), WithinAbs(2.f, kEps_));

        REQUIRE_THAT(rMatrix(3,0), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(rMatrix(3,1), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(rMatrix(3,2), WithinAbs(2.f, kEps_));
        REQUIRE_THAT(rMatrix(3,3), WithinAbs(2.f, kEps_));
    }

    SECTION("Large Values")
    {

        Mat44f const matrixA = {
            {1e6f, 0, 0, 0,
             0, 1e6f, 0, 0,
             0, 0, 1e6f, 0,
             0, 0, 0, 1e6f}
        };
        Mat44f const matrixB = {
            {1e6f, 2e6f, 3e6f, 4e6f,
             5e6f, 6e6f, 7e6f, 8e6f,
             9e6f, 10e6f, 11e6f, 12e6f,
             13e6f, 14e6f, 15e6f, 16e6f}
        };

        Mat44f const rMatrix = matrixA * matrixB;

        REQUIRE_THAT(rMatrix(0,0), WithinAbs(1e12f, kEps_));
        REQUIRE_THAT(rMatrix(0,1), WithinAbs(2e12f, kEps_));
        REQUIRE_THAT(rMatrix(0,2), WithinAbs(3e12f, kEps_));
        REQUIRE_THAT(rMatrix(0,3), WithinAbs(4e12f, kEps_));

        REQUIRE_THAT(rMatrix(1,0), WithinAbs(5e12f, kEps_));
        REQUIRE_THAT(rMatrix(1,1), WithinAbs(6e12f, kEps_));
        REQUIRE_THAT(rMatrix(1,2), WithinAbs(7e12f, kEps_));
        REQUIRE_THAT(rMatrix(1,3), WithinAbs(8e12f, kEps_));

        REQUIRE_THAT(rMatrix(2,0), WithinAbs(9e12f, kEps_));
        REQUIRE_THAT(rMatrix(2,1), WithinAbs(10e12f, kEps_));
        REQUIRE_THAT(rMatrix(2,2), WithinAbs(11e12f, kEps_));
        REQUIRE_THAT(rMatrix(2,3), WithinAbs(12e12f, kEps_));

        REQUIRE_THAT(rMatrix(3,0), WithinAbs(13e12f, kEps_));
        REQUIRE_THAT(rMatrix(3,1), WithinAbs(14e12f, kEps_));
        REQUIRE_THAT(rMatrix(3,2), WithinAbs(15e12f, kEps_));
        REQUIRE_THAT(rMatrix(3,3), WithinAbs(16e12f, kEps_));
    }
    
    SECTION("Negatives")
    {

        Mat44f const matrixA = {
            {-5, 0, 0, 0,
             0, -5, 0, 0,
             0, 0, -5, 0,
             0, 0, 0, -5}
        };
        Mat44f const matrixB = {
            {1, 1, 1, 1,
             1, 1, 1, 1,
             1, 1, 1, 1,
             1, 1, 1, 1}
        };

        Mat44f const rMatrix = matrixA * matrixB;

        REQUIRE_THAT(rMatrix(0,0), WithinAbs(-5.f, kEps_));
        REQUIRE_THAT(rMatrix(0,1), WithinAbs(-5.f, kEps_));
        REQUIRE_THAT(rMatrix(0,2), WithinAbs(-5.f, kEps_));
        REQUIRE_THAT(rMatrix(0,3), WithinAbs(-5.f, kEps_));

        REQUIRE_THAT(rMatrix(1,0), WithinAbs(-5.f, kEps_));
        REQUIRE_THAT(rMatrix(1,1), WithinAbs(-5.f, kEps_));
        REQUIRE_THAT(rMatrix(1,2), WithinAbs(-5.f, kEps_));
        REQUIRE_THAT(rMatrix(1,3), WithinAbs(-5.f, kEps_));

        REQUIRE_THAT(rMatrix(2,0), WithinAbs(-5.f, kEps_));
        REQUIRE_THAT(rMatrix(2,1), WithinAbs(-5.f, kEps_));
        REQUIRE_THAT(rMatrix(2,2), WithinAbs(-5.f, kEps_));
        REQUIRE_THAT(rMatrix(2,3), WithinAbs(-5.f, kEps_));

        REQUIRE_THAT(rMatrix(3,0), WithinAbs(-5.f, kEps_));
        REQUIRE_THAT(rMatrix(3,1), WithinAbs(-5.f, kEps_));
        REQUIRE_THAT(rMatrix(3,2), WithinAbs(-5.f, kEps_));
        REQUIRE_THAT(rMatrix(3,3), WithinAbs(-5.f, kEps_));
    }

    SECTION("Zero")
    {

        Mat44f const matrixA = {
            {0, 0, 0, 0,
             0, 0, 0, 0,
             0, 0, 0, 0,
             0, 0, 0, 0}
        };
        Mat44f const matrixB = {
            {1, 5, 9, 6,
             3, 4, 2, 4,
             4, 12, 7, 7,
             2, 8, 14, 3}
        };

        Mat44f const rMatrix = matrixA * matrixB;

        REQUIRE_THAT(rMatrix(0,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rMatrix(0,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rMatrix(0,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rMatrix(0,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(rMatrix(1,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rMatrix(1,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rMatrix(1,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rMatrix(1,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(rMatrix(2,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rMatrix(2,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rMatrix(2,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rMatrix(2,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(rMatrix(3,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rMatrix(3,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rMatrix(3,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(rMatrix(3,3), WithinAbs(0.f, kEps_));
    }

}