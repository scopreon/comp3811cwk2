#include <catch2/catch_amalgamated.hpp>

#include "../vmlib/mat44.hpp"

TEST_CASE("4x4 Translation", "[translation][mat44]")
{

    static constexpr float kEps_ = 1e-6f;

    using namespace Catch::Matchers;

    SECTION("Generic")
    {
        Vec3f vector = {5, 6, 7};

        Mat44f translated = make_translation(vector);

        REQUIRE_THAT(translated(0,0), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(translated(0,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(0,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(0,3), WithinAbs(5.f, kEps_));

        REQUIRE_THAT(translated(1,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(1,1), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(translated(1,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(1,3), WithinAbs(6.f, kEps_));

        REQUIRE_THAT(translated(2,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(2,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(2,2), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(translated(2,3), WithinAbs(7.f, kEps_));

        REQUIRE_THAT(translated(3,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(3,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(3,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(3,3), WithinAbs(1.f, kEps_));
    }

    SECTION("Negatives")
    {
        Vec3f vector = {-15, -2, -20};

        Mat44f translated = make_translation(vector);

        REQUIRE_THAT(translated(0,0), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(translated(0,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(0,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(0,3), WithinAbs(-15.f, kEps_));

        REQUIRE_THAT(translated(1,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(1,1), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(translated(1,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(1,3), WithinAbs(-2.f, kEps_));

        REQUIRE_THAT(translated(2,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(2,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(2,2), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(translated(2,3), WithinAbs(-20.f, kEps_));

        REQUIRE_THAT(translated(3,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(3,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(3,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(3,3), WithinAbs(1.f, kEps_));
    }

    SECTION("Large Values")
    {
        Vec3f vector = {12e6f, 20e6f, 28e6f};

        Mat44f translated = make_translation(vector);

        REQUIRE_THAT(translated(0,0), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(translated(0,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(0,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(0,3), WithinAbs(12e6f, kEps_));

        REQUIRE_THAT(translated(1,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(1,1), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(translated(1,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(1,3), WithinAbs(20e6f, kEps_));

        REQUIRE_THAT(translated(2,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(2,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(2,2), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(translated(2,3), WithinAbs(28e6f, kEps_));

        REQUIRE_THAT(translated(3,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(3,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(3,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(3,3), WithinAbs(1.f, kEps_));
    }

    SECTION("Large Negatives")
    {
        Vec3f vector = {-12e6f, -20e6f, -28e6f};

        Mat44f translated = make_translation(vector);

        REQUIRE_THAT(translated(0,0), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(translated(0,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(0,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(0,3), WithinAbs(-12e6f, kEps_));

        REQUIRE_THAT(translated(1,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(1,1), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(translated(1,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(1,3), WithinAbs(-20e6f, kEps_));

        REQUIRE_THAT(translated(2,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(2,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(2,2), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(translated(2,3), WithinAbs(-28e6f, kEps_));

        REQUIRE_THAT(translated(3,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(3,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(3,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(translated(3,3), WithinAbs(1.f, kEps_));
    }
    
}