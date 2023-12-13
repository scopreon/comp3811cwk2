#include "spaceship.hpp"
#include "shapes.hpp"
#include <iostream>
#include <math.h>
#include <vector>

#include "../support/program.hpp"
#include "../vmlib/mat44.hpp"
#include "../vmlib/vec3.hpp"
#include "simple_mesh.hpp"

void Spaceship::render(Mat44f projCameraWorld) {
  ShaderProgram prog({{GL_VERTEX_SHADER, "assets/spaceship.vert"},
                      {GL_FRAGMENT_SHADER, "assets/spaceship.frag"}});

  glUseProgram(prog.programId());

  Vec3f lightDir = normalize(Vec3f{0.f, 1.f, -1.f});
  glUniform3fv(5, 1, &lightDir.x);
  glUniform3f(6, 0.9f, 0.9f, 0.6f);
  glUniform3f(7, 0.05f, 0.05f, 0.05f);
  glUniformMatrix4fv(0, 1, GL_TRUE, projCameraWorld.v);

  glUniformMatrix4fv(
      3, 1, GL_TRUE,
      (make_translation({location.x + offset.x, location.y + offset.y,
                         location.z + offset.z}) *
       make_rotation_z(angle) *
       make_translation({-location.x, -location.y, -location.z}))
          .v);
  glBindVertexArray(spaceshipVAO);
  glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

void Spaceship::update(float ts) {

  constexpr float kPi_ = 3.1415926f;
  offset.y = pow(ts / 3, 2);
  offset.x = pow(ts / 7, 3);

  float angleNumerator = -offset.y + pow((ts + 0.01) / 3, 2);
  float angleDenominator = -offset.x + pow((ts + 0.01) / 7, 3);

  angle = atan(angleNumerator / angleDenominator) - kPi_ / 2;
}

Spaceship::Spaceship(std::size_t aSubdivs, Mat44f aPreTransform) {

  Vec4f p4{0, 0, 0, 1.f};
  Vec4f t = aPreTransform * p4;
  t /= t.w;

  Vec3f pTransformed{t.x, t.y, t.z};
  location = pTransformed;

  offset = {0, 0, 0};

  constexpr float kPi_ = 3.1415926f;

  Vec3f colorWings({0.94f, 0.28f, 0.21f});
  Vec3f colorCone({0.94f, 0.28f, 0.21f});
  Vec3f colorBase1({1.f, 1.f, 1.f});
  Vec3f colorBase2({0.47f, 0.69f, 0.91f});
  Vec3f colorBase3({1.f, 1.f, 1.f});
  Vec3f colorWindow({0, 0, 0});
  std::vector<SimpleMeshData> shapes;

  shapes.push_back(make_cylinder(true, 16, colorBase1,
                                 aPreTransform * make_rotation_z(kPi_ / 2) *
                                     make_scaling(2, 1, 1)));

  shapes.push_back(make_cylinder(true, 16, colorBase2,
                                 aPreTransform * make_translation({0, 2, 0}) *
                                     make_rotation_z(kPi_ / 2) *
                                     make_scaling(2, 1, 1)));

  shapes.push_back(make_cylinder(true, 16, colorBase3,
                                 aPreTransform * make_translation({0, 4, 0}) *
                                     make_rotation_z(kPi_ / 2) *
                                     make_scaling(2, 1, 1)));

  shapes.push_back(make_cone(true, 16, colorCone,
                             aPreTransform * make_translation({0, 9.5f, 0}) *
                                 make_rotation_z(kPi_ / 2) *
                                 make_scaling(3.5f, 1, 1) *
                                 make_rotation_z(kPi_)));

  shapes.push_back(make_prism(true, 3, colorWings,
                              aPreTransform * make_translation({0.95f, 0, 0}) *
                                  make_scaling(0.6f, 2, 1)));
  shapes.push_back(make_prism(
      true, 3, colorWings,
      aPreTransform * make_rotation_y(-kPi_ * 2.f / 3.f) *
          make_translation({0.95f, 0, 0}) * make_scaling(0.7f, 2, 1)));

  shapes.push_back(make_prism(
      true, 3, colorWings,
      aPreTransform * make_rotation_y(kPi_ * 2.f / 3.f) *
          make_translation({0.95f, 0, 0}) * make_scaling(0.7f, 2, 1)));

  shapes.push_back(make_cylinder(true, 16, colorWindow,
                                 aPreTransform * make_translation({-1, 4, 0}) *
                                     make_scaling(2, 0.5, 0.5)));
  SimpleMeshData spaceship;
  for (auto shape : shapes) {
    std::cout << "adding spaceship" << std::endl;
    std::cout << shape.positions.size() << std::endl;
    ;
    spaceship = concatenate(spaceship, shape);
  }
  std::cout << spaceship.positions.size() << std::endl;

  numVertices = spaceship.positions.size();
  spaceshipVAO = create_vao(spaceship);
}
