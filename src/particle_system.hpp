#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

class ParticleSystem {
public:
	~ParticleSystem();
	void init();
	void draw(const GLuint program);

private:
	GLuint vbo;
	GLuint instance_vbo;
	glm::vec2 translations[100];
};