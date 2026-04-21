#include "octree.h"

#include "rendering/icosphere.h"

Box::Box() : object(nullptr) {
    children.fill(nullptr);
}


int Box::findOctant(const glm::vec3& point) const {
    int oct = 0;
    if (point.x >= pos.x) oct |= 4;
    if (point.y >= pos.y) oct |= 2;
    if (point.z >= pos.z) oct |= 1;
    return oct;
}

bool Box::isLeaf() const {
    return children[0] == nullptr;
}

bool Box::contains(const glm::vec3& point) const {
    return point.x >= pos.x - halfExtent.x && point.x < pos.x + halfExtent.x &&
        point.y >= pos.y - halfExtent.y && point.y < pos.y + halfExtent.y &&
        point.z >= pos.z - halfExtent.z && point.z < pos.z + halfExtent.z;
}

void Box::insert(Arena& arena, const Body* newobj)
{
    if (!contains(newobj->loc)) return;
    
    com = (com * (float)mass + newobj->loc * newobj->mass) / float(mass + newobj->mass);
    mass += newobj->mass;

    if (!isLeaf()) {
        children[findOctant(newobj->loc)]->insert(arena, newobj);
        return;
    }
    
    if (object == nullptr)
    {
        mass = newobj->mass;
        com = newobj->loc;
        object = newobj;
        return;
    }

    const Body* old = object;
    object = nullptr;

    for (uint8_t i = 0; i < 8; i++)
    {
        glm::vec3 newpos = pos;
        newpos.x += halfExtent.x * (i & 4 ? 0.5f : -0.5f);
        newpos.y += halfExtent.y * (i & 2 ? 0.5f : -0.5f);
        newpos.z += halfExtent.z * (i & 1 ? 0.5f : -0.5f);
        children[i] = arena.create<Box>();
        children[i]->pos = newpos;
        children[i]->halfExtent = halfExtent * 0.5f;
    }

    children[findOctant(old->loc)]->insert(arena, old);
    children[findOctant(newobj->loc)]->insert(arena, newobj);
}


void Box::draw(Camera& cam, bool priority)
{
    for (Box* child : children) {
        if (child)
            child->draw(cam, priority);
    }
}

Box* rebuild_tree(Arena& arena, Box* box, std::vector<Body>& bodies)
{
    arena.clear();

    Box* new_tree = arena.create<Box>();
    new_tree->halfExtent = glm::vec3(100000);
	new_tree->pos = glm::vec3(0.0f);

    for (Body& body : bodies)
    {
        body.acc = glm::vec3(0.0f);
        body.force = glm::vec3(0.0f);
        new_tree->insert(arena, &body);
    }

    return new_tree;
}

glm::vec3 calc(float G, glm::vec3 curr_pos, float current_mass, glm::vec3 other_pos, float other_mass)
{
    glm::vec3 r_vec = other_pos - curr_pos;
    float dist = glm::length(r_vec) + 0.0001f;
    return (r_vec * G * current_mass * other_mass) / float(std::pow(dist, 3));
}

glm::vec3 Box::calculateForce(float G, float theta, Body* body)
{
    glm::vec3 force = glm::vec3(0.0f);

    if (isLeaf()) {
        if (object == nullptr || object == body) return force;
        return calc(G, body->loc, body->mass, object->loc, object->mass);
    }
    
    float r = glm::distance(com, body->loc);

    float d = halfExtent.x * 2.0f;

    if (d/r < theta) {
        return calc(G, body->loc, body->mass, com, mass);
    }

    for (Box* child : children) {
        if (child)
            force += child->calculateForce(G, theta, body);
    }
    
    return force;
}