
#include "tiny_ecs_registry.hpp"

#include <random>

class AISystem {
public:
	AISystem(std::default_random_engine& rng);
	void step(float elapsed_ms);

private:
	void moveTowardsPlayer(Entity enemy, vec3 playerPosition);
	
	void boarBehaviour(Entity boar, vec3 playerPosition);
	void barbarianBehaviour(Entity barbarian, vec3 playerPosition);

	// Archer functions
	void archerBehaviour(Entity entity, vec3 playerPosition, float elapsed_ms);
	void shootArrow(Entity shooter, vec3 targetPos);

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};