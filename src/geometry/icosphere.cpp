#include "geometry/icosphere.h"

const float GOLDEN_RATIO = 1.61803398875f;

const float X = 1.0f / sqrt(1 + GOLDEN_RATIO * GOLDEN_RATIO);
const float Z = GOLDEN_RATIO * X;
const float N = 0.0f;

static const std::vector<float> ICOSPHERE_VERTICES = {
    -X, N,  Z, X, N,  Z,  -X, N, -Z, X,  N, -Z, N, Z,  X, N,  Z,  -X,
    N,  -Z, X, N, -Z, -X, Z,  X, N,  -Z, X, N,  Z, -X, N, -Z, -X, N};

static const std::vector<uint32_t> ICOSPHERE_TRIANGLES = {
    0,  4, 1, 0, 9, 4, 9, 5,  4, 4, 5,  8,  4,  8, 1, 8,  10, 1,  8, 3,
    10, 5, 3, 8, 5, 2, 3, 2,  7, 3, 7,  10, 3,  7, 6, 10, 7,  11, 6, 11,
    0,  6, 0, 1, 6, 6, 1, 10, 9, 0, 11, 9,  11, 2, 9, 2,  5,  7,  2, 11};

int getMidPoint(std::unordered_map<uint64_t, int> &map,
                std::vector<float> &vertices, int p1, int p2) {
  uint64_t smIndex = (p1 < p2) ? p1 : p2;
  uint64_t lgIndex = (p1 < p2) ? p2 : p1;

  uint64_t key = (smIndex << 32) + lgIndex;

  auto [it, inserted] = map.insert({key, vertices.size() / 3});
  if (inserted) {
    glm::vec3 edge0 = glm::vec3(vertices[(p1 * 3)], vertices[(p1 * 3) + 1],
                                vertices[(p1 * 3) + 2]);
    glm::vec3 edge1 = glm::vec3(vertices[(p2 * 3)], vertices[(p2 * 3) + 1],
                                vertices[(p2 * 3) + 2]);
    auto midpoint = glm::normalize(edge0 + edge1);
    vertices.push_back(midpoint.x);
    vertices.push_back(midpoint.y);
    vertices.push_back(midpoint.z);
  }

  return it->second;
}

std::vector<uint32_t> subdivide(std::vector<float> &vertices,
                                std::vector<uint32_t> &triangles) {
  std::unordered_map<uint64_t, int> map;
  std::vector<uint32_t> result;

  uint32_t mid[3];
  for (int i = 0; i < triangles.size(); i += 3) {
    for (int edge = 0; edge < 3; edge++) {
      mid[edge] = getMidPoint(map, vertices, triangles[i + edge],
                              triangles[i + ((edge + 1) % 3)]);
    }

    result.insert(result.end(), {triangles[i + 0], mid[0], mid[2]});
    result.insert(result.end(), {triangles[i + 1], mid[1], mid[0]});
    result.insert(result.end(), {triangles[i + 2], mid[2], mid[1]});
    result.insert(result.end(), {mid[0], mid[1], mid[2]});
  }

  return result;
}

IcoSphere::Geometry GenerateIcoSphere(uint8_t subdivision_count) {
  std::vector<float> vertices = ICOSPHERE_VERTICES;
  std::vector<uint32_t> triangles = ICOSPHERE_TRIANGLES;

  for (int i = 0; i < subdivision_count; i++)
    triangles = subdivide(vertices, triangles);

  return {subdivision_count, vertices, triangles};
}
