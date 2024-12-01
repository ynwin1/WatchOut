#pragma once

#include "tiny_ecs_registry.hpp"
#include <random>

class RenderSystem;

enum class PARTICLE {
	smoke
};

class ParticleSystem {
public:
	~ParticleSystem();
	void init(RenderSystem* renderer);
	void draw(const GLuint program);
	void step(float elapsed_ms);

	// Create particles
	Entity createSmokeParticle(vec3 position);

private:
	RenderSystem* renderer;
	GLuint transforms_vbo;
	std::vector<mat3> transforms;

	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist;
};