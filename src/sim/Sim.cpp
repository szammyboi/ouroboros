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
	m_Tree->halfExtent = glm::vec3(50000);
	m_Tree->pos = glm::vec3(0.0f);
}

void Sim::step(float dt)
{
	if (!m_Playing)
		return;
		
	m_Tree = rebuild_tree(m_Arena, m_Tree, Bodies);

	/*for (Body& body : Bodies) {
		body.acc = m_Tree->calculateForce(config.G, 1.0f, &body) / body.mass;
	}*/

	std::for_each(
		std::execution::par,
		Bodies.begin(),
		Bodies.end(),
		[&](Body& body)
		{
			body.acc = m_Tree->calculateForce(config.G, 1.0f, &body) / body.mass;
		}
	);

	/*for (Body& curr_body : Bodies)
	{
		curr_body.vel += curr_body.acc * dt;
		curr_body.loc += curr_body.vel * dt;
	}*/

	// O(n^2) calculation, will switch to Barnes-Hut code as n increases
	/*for (Body& curr_body: Bodies) {
		curr_body.acc = glm::vec3(0.0f);

		for (Body& other_body: Bodies) {
			if (&curr_body == &other_body) continue;

			glm::vec3 r_vec  = other_body.loc - curr_body.loc;
			double r_dist = glm::length(r_vec);

			glm::vec3 force_mag = r_vec * float(config.G * curr_body.mass * other_body.mass) / float(r_dist * r_dist * r_dist);
			curr_body.acc += (force_mag) / curr_body.mass;

		}
	}	*/
	
	std::for_each(
		std::execution::par,
		Bodies.begin(),
		Bodies.end(),
		[&](Body& body)
		{
			body.vel += body.acc * dt;
			body.loc += body.vel * dt;
		}
	);
}
