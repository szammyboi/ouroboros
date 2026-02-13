#include "Body.h"
#include <vector>

// Put more constants here
struct SimConfig
{
	double G { 6.67430e-11 }; // Nm^2/kg^2
	double simDt { 1.0 / 240.0 };
};

class Sim
{
public:
	Sim();
	void step(float dt);
	SimConfig config;
	inline void add_body(Body new_body){ Bodies.push_back(new_body);}

private:
	std::vector<Body> Bodies;
};
