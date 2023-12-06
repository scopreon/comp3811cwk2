#include "spaceship.hpp"
#include "shapes.hpp"

#include "../vmlib/mat44.hpp"
#include "../vmlib/vec3.hpp"
#include "simple_mesh.hpp"

SimpleMeshData make_spaceship(std::size_t aSubdivs, Mat44f aPreTransform) {
  constexpr float kPi_ = 3.1415926f;

  Vec3f colorWings({0.94f, 0.28f, 0.21f});
  Vec3f colorCone({0.94f, 0.28f, 0.21f});
  Vec3f colorBase1({1.f, 1.f, 1.f});
  Vec3f colorBase2({0.47f, 0.69f, 0.91f});
  Vec3f colorBase3({1.f, 1.f, 1.f});
  Vec3f colorWindow({0, 0, 0});

  SimpleMeshData c1 = make_cylinder(true, 16, colorBase1,
                                    aPreTransform * make_rotation_z(kPi_ / 2) *
                                        make_scaling(2, 1, 1));
  SimpleMeshData c2 =
      make_cylinder(true, 16, colorBase2,
                    aPreTransform * make_translation({0, 2, 0}) *
                        make_rotation_z(kPi_ / 2) * make_scaling(2, 1, 1));
  SimpleMeshData c3 =
      make_cylinder(true, 16, colorBase3,
                    aPreTransform * make_translation({0, 4, 0}) *
                        make_rotation_z(kPi_ / 2) * make_scaling(2, 1, 1));
  SimpleMeshData t1 =
      make_cone(true, 16, colorCone,
                aPreTransform * make_translation({0, 9.5f, 0}) *
                    make_rotation_z(kPi_ / 2) * make_scaling(3.5f, 1, 1) *
                    make_rotation_z(kPi_));

  SimpleMeshData p1 =
      make_prism(true, 3, colorWings,
                 aPreTransform * make_translation({0.95f, 0, 0}) *
                     make_scaling(0.6f, 2, 1));
  SimpleMeshData p2 = make_prism(
      true, 3, colorWings,
      aPreTransform * make_rotation_y(-kPi_ * 2.f / 3.f) *
          make_translation({0.95f, 0, 0}) * make_scaling(0.7f, 2, 1));
  SimpleMeshData p3 = make_prism(
      true, 3, colorWings,
      aPreTransform * make_rotation_y(kPi_ * 2.f / 3.f) *
          make_translation({0.95f, 0, 0}) * make_scaling(0.7f, 2, 1));

  SimpleMeshData w1 = make_cylinder(true, 16, colorWindow,
                                    aPreTransform * make_translation({-1, 4, 0}) *
                                        make_scaling(2, 0.5, 0.5));
  return concatenate(
      concatenate(
          concatenate(
              concatenate(concatenate(concatenate(concatenate(c1, c2), c3), t1),
                          p1),
              p2),
          p3),
      w1);
}
