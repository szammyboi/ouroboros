/*
    Derived from: https://github.com/brandonpelfrey/SimpleOctree/blob/master/Octree.h
*/

#pragma once

#include <array>
#include <vector>
#include <glm/glm.hpp>
#include <iostream>

#include "sim/Body.h"
#include "rendering/octree.h"
#include "geometry/arena.h"

struct Box {
    glm::vec3 pos = glm::vec3(0.0f);
    glm::vec3 halfExtent = glm::vec3(0.0f);

    const Body* object;
    std::array<Box*, 8> children;

    double mass = 0.0f;
    glm::vec3 com = glm::vec3(0.0f);

    Box();

    int findOctant(const glm::vec3& point) const;

    bool isLeaf() const;

    bool contains(const glm::vec3& point) const;

    void insert(Arena& arena, const Body* newobj);

    glm::vec3 calculateForce(float G, float theta, Body* body);

    void draw(Camera& cam, bool priority);
};

Box* rebuild_tree(Arena& arena, Box* box, std::vector<Body>& bodies);