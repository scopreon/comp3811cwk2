#include "shapes.hpp"

#include "../vmlib/mat33.hpp"
#include "../vmlib/mat44.hpp"
#include "simple_mesh.hpp"
#include <iostream>


// cross product for Vectors
inline Vec3f cross(Vec3f aLeft, Vec3f aRight) noexcept {
  return Vec3f{aLeft.y * aRight.z - aLeft.z * aRight.y,
               aLeft.z * aRight.x - aLeft.x * aRight.z,
               aLeft.x * aRight.y - aLeft.y * aRight.x};
}

SimpleMeshData make_cylinder(bool aCapped, std::size_t aSubdivs, Vec3f aColor,
                             Mat44f aPreTransform) {
  Mat33f const N = mat44_to_mat33(transpose(invert(aPreTransform)));
  std::vector<Vec2f> texcoords;
  std::vector<Vec3f> pos;
  std::vector<Vec3f> normals; // Add a vector to store normals
  float prevY = std::cos(0.f);
  float prevZ = std::sin(0.f);

  for (std::size_t i = 0; i < aSubdivs; ++i) {
    float const angle =
        (i + 1) / static_cast<float>(aSubdivs) * 2.f * 3.1415926f;
    float y = std::cos(angle);
    float z = std::sin(angle);

    Vec3f vertex1{0.f, prevY, prevZ};
    Vec3f vertex2{0.f, y, z};
    Vec3f vertex3{1.f, prevY, prevZ};
    Vec3f vertex4{1.f, y, z};

    Vec3f normal1 = normalize(cross(vertex2 - vertex4, vertex2 - vertex3));
    Vec3f normal2 = normal1;
    pos.push_back(vertex1);
    pos.push_back(vertex2);
    pos.push_back(vertex3);

    pos.push_back(vertex2);
    pos.push_back(vertex4);
    pos.push_back(vertex3);

    Vec2f texcoord1{static_cast<float>(i) / (aSubdivs - 1), 0.f};
    Vec2f texcoord2{static_cast<float>(i) / (aSubdivs - 1), 1.f};

    texcoords.push_back(texcoord1);
    texcoords.push_back(texcoord2);
    texcoords.push_back(texcoord1);

    texcoords.push_back(texcoord2);
    texcoords.push_back(texcoord2);
    texcoords.push_back(texcoord1);

    // Store normals for each vertex
    normals.push_back(normal1);
    normals.push_back(normal1);
    normals.push_back(normal1);

    normals.push_back(normal2);
    normals.push_back(normal2);
    normals.push_back(normal2);

    if (aCapped) {
      pos.push_back(Vec3f{1.f, prevY, prevZ});
      pos.push_back(Vec3f{1.f, y, z});
      pos.push_back(Vec3f{1.f, 0, 0});

      pos.push_back(Vec3f{0.f, prevY, prevZ});
      pos.push_back(Vec3f{0.f, y, z});
      pos.push_back(Vec3f{0.f, 0, 0});

      Vec3f normalCap = Vec3f{1,0,0};
      Vec3f normalCap2 = -normalCap;

      normals.push_back(normalCap);
      normals.push_back(normalCap);
      normals.push_back(normalCap);

      normals.push_back(normalCap2);
      normals.push_back(normalCap2);
      normals.push_back(normalCap2);

      Vec2f texcoordCap1{1.f, prevY};
      Vec2f texcoordCap2{1.f, y};
      Vec2f texcoordCap3{0.5f, 0.5f};

      texcoords.push_back(texcoordCap1);
      texcoords.push_back(texcoordCap2);
      texcoords.push_back(texcoordCap3);

      texcoords.push_back(texcoordCap2);
      texcoords.push_back(texcoordCap2);
      texcoords.push_back(texcoordCap3);
    }

    prevY = y;
    prevZ = z;
  }

  // Transform and normalize normals
  for (auto &normal : normals) {
    normal = normalize(N * normal);
  }

  // Transform positions
  for (auto &p : pos) {
    Vec4f p4{p.x, p.y, p.z, 1.f};
    Vec4f t = aPreTransform * p4;
    t /= t.w;

    Vec3f pTransformed{t.x, t.y, t.z};
    p = pTransformed;
  }

  std::vector col(pos.size(), aColor);

  return SimpleMeshData{std::move(pos), std::move(col), std::move(normals),
                        std::move(texcoords)};
}

SimpleMeshData make_cone(bool aCapped, std::size_t aSubdivs, Vec3f aColor,
                         Mat44f aPreTransform) {
  Mat33f const N = mat44_to_mat33(transpose(invert(aPreTransform)));
  std::vector<Vec2f> texcoords;
  std::vector<Vec3f> pos;
  std::vector<Vec3f> normals; // Add a vector to store normals
  float prevY = std::cos(0.f);
  float prevZ = std::sin(0.f);

  for (std::size_t i = 0; i < aSubdivs; ++i) {
    float const angle =
        (i + 1) / static_cast<float>(aSubdivs) * 2.f * 3.1415926f;
    float y = std::cos(angle);
    float z = std::sin(angle);

    Vec3f vertex2{0.f, 0, 0};
    Vec3f vertex3{1.f, prevY, prevZ};
    Vec3f vertex4{1.f, y, z};

    // Calculate normals for each triangle
    Vec3f normal2 = normalize(cross(vertex2 - vertex4, vertex2 - vertex3));
    Vec3f normal1 = normal2;
    // Two triangles (= 3*2 positions) create one segment of the cylinder’s
    // shell.
    pos.push_back(vertex2);
    pos.push_back(vertex4);
    pos.push_back(vertex3);

    Vec2f texcoord1{static_cast<float>(i) / (aSubdivs - 1), 0.f};
    Vec2f texcoord2{static_cast<float>(i) / (aSubdivs - 1), 1.f};

    texcoords.push_back(texcoord2);
    texcoords.push_back(texcoord2);
    texcoords.push_back(texcoord1);

    // Store normals for each vertex
    normals.push_back(normal2);
    normals.push_back(normal2);
    normals.push_back(normal2);

    if (aCapped) {
      pos.push_back(Vec3f{1.f, prevY, prevZ});
      pos.push_back(Vec3f{1.f, y, z});
      pos.push_back(Vec3f{1.f, 0, 0});

      // Normals for capped triangles
      Vec3f normalCap =
          normalize(cross(Vec3f{0.f, 0.f, 1.f}, vertex4 - vertex2));

      normals.push_back(normalCap);
      normals.push_back(normalCap);
      normals.push_back(normalCap);

      Vec2f texcoordCap1{1.f, prevY};
      Vec2f texcoordCap2{1.f, y};
      Vec2f texcoordCap3{0.5f, 0.5f};

      texcoords.push_back(texcoordCap1);
      texcoords.push_back(texcoordCap2);
      texcoords.push_back(texcoordCap3);
    }

    prevY = y;
    prevZ = z;
  }

  // Transform and normalize normals
  for (auto &normal : normals) {
    normal = normalize(N * normal);
  }

  // Transform positions
  for (auto &p : pos) {
    Vec4f p4{p.x, p.y, p.z, 1.f};
    Vec4f t = aPreTransform * p4;
    t /= t.w;

    Vec3f pTransformed{t.x, t.y, t.z};
    p = pTransformed;
  }

  std::vector col(pos.size(), aColor);

  return SimpleMeshData{std::move(pos), std::move(col), std::move(normals),
                        std::move(texcoords)};
}

// prism is just cylinder with n sides
SimpleMeshData make_prism(bool aCapped, std::size_t aSubdivs, Vec3f aColor,
                          Mat44f aPreTransform) {
  return make_cylinder(aCapped, aSubdivs, aColor, aPreTransform);
}
