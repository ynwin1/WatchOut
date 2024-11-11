
#include "tiny_ecs_registry.hpp"

#include <random>

class AISystem {
public:
	AISystem(std::default_random_engine& rng);
	void step(float elapsed_ms);

private:
	const float LIGHTENING_RADIUS = 200.f;

	void moveTowardsPlayer(Entity enemy, vec3 playerPosition, float elapsed_ms);
	vec2 chooseDirection(Motion& motion, vec3 playerPosition);
	bool pathClear(Motion& motion, vec2 direction, float howFar, const std::vector<Entity> &obstacles, float& clearDistance);
	
	void boarBehaviour(Entity boar, vec3 playerPosition, float elapsed_ms);
	void barbarianBehaviour(Entity barbarian, vec3 playerPosition, float elapsed_ms);

	// Archer functions
	void archerBehaviour(Entity entity, vec3 playerPosition, float elapsed_ms);
	void shootArrow(Entity shooter, vec3 targetPos);

	// Wizard functions
	void wizardBehaviour(Entity entity, vec3 playerPosition, float elapsed_ms);
	void shootFireball(Entity shooter, vec3 targetPos);
	void triggerLightening(vec3 targetPos);

	// Wizard AI helper functions
	vec3 selectWizardAttack(Entity& wizard, vec3 playerPosition, float BUFFER, float elapsed_ms);
	void makeLighteningDecision(Wizard& wizard, float LIGHTNING_PREPARE_TIME, float elapsed_ms);

	vec2 randomDirection();

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};