#include "Sim.h"
#include "spdlog/spdlog.h"

#include <thread>
#include <vector>
#include <execution>
#include <algorithm>

// this needs to not be hardcoded
// we also need to properly implement the leapfrog with a half initial step
// the half, recalc, then calc
// combined with a epsilon and ...
// also should probably calculate the sizeof the box each time with max positions
// we need to possibly make the boxes contain multiple points
// collisions, ...
Sim::Sim()
	: m_Arena(sizeof(Box) * 10000000)
{
	std::cout << sizeof(Box) * 10000000;
	spdlog::info("Sim Created...");

	m_Tree = m_Arena.create<Box>();
	m_Tree->halfExtent = glm::vec3(500'000);
	m_Tree->pos = glm::vec3(0.0f);
}

void Sim::step(float dt)
{
	float dt_scaled = dt * config.timeMultiplier;
	if (!m_Playing)
		return;

    for (auto& body : Bodies)
        body.vel += body.acc * (0.5f * dt_scaled);

    for (auto& body : Bodies)
        body.loc += body.vel * dt_scaled;
		
	m_Tree = rebuild_tree(m_Arena, m_Tree, Bodies);

	std::for_each(
		std::execution::par,
		Bodies.begin(),
		Bodies.end(),
		[&](Body& body)
		{
			body.acc = m_Tree->calculateForce(config.G, 2.0f, &body) / body.mass;
		}
	);

    for (auto& body : Bodies)
        body.vel += body.acc * (0.5f * dt_scaled);
	
	/*std::for_each(
		std::execution::par,
		Bodies.begin(),
		Bodies.end(),
		[&](Body& body)
		{
			body.vel += body.acc * dt;
			body.loc += body.vel * dt;
		}
	);*/
}
