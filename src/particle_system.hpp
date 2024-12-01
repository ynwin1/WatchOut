#pragma once

#include "tiny_ecs_registry.hpp"
#include <random>

class RenderSystem;

class ParticleSystem {
public:
	~ParticleSystem();
	void init(RenderSystem* renderer);
	void draw(const GLuint program);
	void step(float elapsed_ms);

	// Create particles
	Entity createSmokeParticle(vec3 position, vec2 size);
	Entity createDashParticle(vec3 position, vec2 size);

private:
	RenderSystem* renderer;
	std::unordered_map<PARTICLE, GLuint> vbos;
	std::unordered_map<PARTICLE, std::vector<mat3>> transforms;

	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist;

	// Draw functions
	void applyBindings(const GLuint program, mat3& projection, PARTICLE particle_type);
	void drawSmoke(const GLuint program, mat3& projection);
	void drawDash(const GLuint program, mat3& projection);
};