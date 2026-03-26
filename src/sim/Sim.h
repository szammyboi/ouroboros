#pragma once

#include "Body.h"
#include <vector>

#include "geometry/octree.h"
#include "geometry/arena.h"

// Put more constants here
struct SimConfig
{
	float G { 0.0000001 }; // Nm^2/kg^2
	double simDt { 1.0 / 120.0 };
};

class Sim
{
public:
	Sim();
	void step(float dt);
	SimConfig config;
	inline void add_body(Body new_body){ 
		Bodies.push_back(new_body); 
		m_Tree->insert(m_Arena, &Bodies[Bodies.size()-1]);
	}

	void Toggle() {
		m_Playing = !m_Playing;
	}

	bool isPlaying() {
		return m_Playing;
	}
public:
	Arena m_Arena;
	std::vector<Box*> boxes;
	std::vector<Body> Bodies;
	Box* m_Tree;
	bool m_Playing = true;
};
