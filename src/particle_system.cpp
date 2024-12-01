#include "particle_system.hpp"
#include "render_system.hpp"
#include "physics_system.hpp"
#include "world_init.hpp"

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::init(RenderSystem* renderer)
{
    this->renderer = renderer;

    glGenBuffers(1, &transforms_vbo);
}

void ParticleSystem::draw(const GLuint program)
{
    mat3 projection = renderer->createProjectionMatrix();

    size_t amount = registry.particles.size();

    transforms.clear();
    transforms.reserve(amount);

    for (Particle& particle : registry.particles.components) {
        Transform transform;
        transform.translate(worldToVisual(particle.position));
        transform.rotate(particle.angle);
        transform.scale(particle.scale);
        transforms.push_back(transform.mat);
    }

    glBindBuffer(GL_ARRAY_BUFFER, transforms_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat3) * amount, transforms.data(), GL_STATIC_DRAW);

    glUseProgram(program);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SPRITE]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)(3 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, transforms_vbo);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3), (void*)0);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3), (void*)(1 * sizeof(vec3)));
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3), (void*)(2 * sizeof(vec3)));
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);

    glActiveTexture(GL_TEXTURE0);
    GLuint texture_id = renderer->texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::SMOKE];
    glBindTexture(GL_TEXTURE_2D, texture_id);

    GLuint projection_loc = glGetUniformLocation(program, "projection");
    glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr, amount);
    gl_has_errors();
}

void ParticleSystem::step(float elapsed_ms)
{
    for (size_t i = 0; i < registry.particles.size(); i++) {
        Entity entity = registry.particles.entities[i];
        Particle& particle = registry.particles.components[i];
        particle.position += particle.velocity * elapsed_ms;
        particle.velocity.z -= GRAVITATIONAL_CONSTANT * particle.gravity;
        particle.life -= elapsed_ms;
        if (particle.life < 0) {
            registry.remove_all_components_of(entity);
        }
    }
}

Entity ParticleSystem::createSmokeParticle(vec3 position)
{
    Entity entity = Entity();

    Particle& particle = registry.particles.emplace(entity);
    particle.position = position;
    particle.position.z += 40;
    particle.velocity = vec3(0, 0, 0.1);
    particle.scale = vec2(20, 20);
    particle.gravity = 0;
    particle.life = 1000;

    return entity;
}
