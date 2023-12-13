#include <catch2/catch_amalgamated.hpp>

#include "../vmlib/mat44.hpp"

TEST_CASE("4x4 Rotation, Varied Axes", "[rotation][mat44]")
{

    static constexpr float kEps_ = 1e-6f;

    using namespace Catch::Matchers;

    SECTION("Identity X-Rotate Zero")
    {
        auto const identityX = make_rotation_x(0.f);
        
        REQUIRE_THAT(identityX(0,0), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(identityX(0,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(0,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(0,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(1,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(1,1), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(identityX(1,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(1,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(2,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(2,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(2,2), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(identityX(2,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(3,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,3), WithinAbs(1.f, kEps_));
    }

    SECTION("Identity X-Rotate 90")
    {
        auto const identityX = make_rotation_x(3.1415926f/2.f);
        
        REQUIRE_THAT(identityX(0,0), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(identityX(0,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(0,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(0,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(1,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(1,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(1,2), WithinAbs(-1.f, kEps_));
        REQUIRE_THAT(identityX(1,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(2,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(2,1), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(identityX(2,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(2,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(3,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,3), WithinAbs(1.f, kEps_));
    }

    SECTION("Identity Y-Rotate Zero")
    {
        auto const identityX = make_rotation_y(0.f);
        
        REQUIRE_THAT(identityX(0,0), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(identityX(0,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(0,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(0,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(1,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(1,1), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(identityX(1,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(1,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(2,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(2,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(2,2), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(identityX(2,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(3,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,3), WithinAbs(1.f, kEps_));
    }

    SECTION("Identity Y-Rotate 90")
    {
        auto const identityX = make_rotation_y(3.1415926f/2.f);
        
        REQUIRE_THAT(identityX(0,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(0,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(0,2), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(identityX(0,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(1,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(1,1), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(identityX(1,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(1,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(2,0), WithinAbs(-1.f, kEps_));
        REQUIRE_THAT(identityX(2,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(2,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(2,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(3,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,3), WithinAbs(1.f, kEps_));
    }

    SECTION("Identity Z-Rotate Zero")
    {
        auto const identityX = make_rotation_z(0.f);
        
        REQUIRE_THAT(identityX(0,0), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(identityX(0,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(0,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(0,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(1,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(1,1), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(identityX(1,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(1,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(2,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(2,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(2,2), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(identityX(2,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(3,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,3), WithinAbs(1.f, kEps_));
    }

    SECTION("Identity Z-Rotate 90")
    {
        auto const identityX = make_rotation_z(3.1415926f/2.f);
        
        REQUIRE_THAT(identityX(0,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(0,1), WithinAbs(-1.f, kEps_));
        REQUIRE_THAT(identityX(0,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(0,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(1,0), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(identityX(1,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(1,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(1,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(2,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(2,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(2,2), WithinAbs(1.f, kEps_));
        REQUIRE_THAT(identityX(2,3), WithinAbs(0.f, kEps_));

        REQUIRE_THAT(identityX(3,0), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,1), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,2), WithinAbs(0.f, kEps_));
        REQUIRE_THAT(identityX(3,3), WithinAbs(1.f, kEps_));
    }
}
