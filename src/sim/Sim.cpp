#include "Sim.h"
#include "spdlog/spdlog.h"

Sim::Sim()
{
	spdlog::info("Sim Created...");
}

void Sim::step(float dt)
{
	// O(n^2) calculation, will switch to Barnes-Hut code as n increases
	for (Body& curr_body: Bodies) {
		glm::vec3 acceleration(0.0f);

		for (Body& other_body: Bodies) {
			if (&curr_body == &other_body) continue;

			glm::vec3 r_vec  = other_body.loc - curr_body.loc;
			double r_dist = glm::length(r_vec);

			float force_mag = (config.G * curr_body.mass * other_body.mass) / (r_dist * r_dist);
			acceleration += (force_mag * glm::normalize(r_vec)) / curr_body.mass;

		}

		curr_body.vel += acceleration * (dt / 2.0f);
		curr_body.loc += curr_body.vel * dt;
	}		
}
