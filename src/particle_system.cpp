#include "particle_system.hpp"
#include "render_system.hpp"
#include "physics_system.hpp"
#include "world_init.hpp"

ParticleSystem::~ParticleSystem()
{
    for (auto& it : vbos) {
        glDeleteBuffers(1, &it.second);
    }
}

void ParticleSystem::init(RenderSystem* renderer)
{
    this->renderer = renderer;
    this->rng = std::default_random_engine(std::random_device()());

    glGenBuffers(1, &vbos[PARTICLE::SMOKE]);
    glGenBuffers(1, &vbos[PARTICLE::DASH]);
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

void ParticleSystem::draw(const GLuint program)
{
    mat3 projection = renderer->createProjectionMatrix();
    transforms.clear();
    size_t amount = registry.particles.size();
    transforms[PARTICLE::SMOKE].reserve(amount);
    transforms[PARTICLE::DASH].reserve(amount);
    for (Particle& particle : registry.particles.components) {
        Transform transform;
        transform.translate(worldToVisual(particle.position));
        transform.rotate(particle.angle);
        transform.scale(particle.scale);
        transforms[particle.type].push_back(transform.mat);
    }

    drawDash(program, projection);
    drawSmoke(program, projection);
}

void ParticleSystem::applyBindings(const GLuint program, mat3& projection, PARTICLE particle_type) 
{
    glBindBuffer(GL_ARRAY_BUFFER, vbos[particle_type]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat3) * transforms[particle_type].size(), transforms[particle_type].data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SPRITE]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)(3 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, vbos[particle_type]);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3), (void*)0);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3), (void*)(1 * sizeof(vec3)));
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3), (void*)(2 * sizeof(vec3)));
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);

    GLuint projection_loc = glGetUniformLocation(program, "projection");
    glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
}

void ParticleSystem::drawSmoke(const GLuint program, mat3& projection)
{
    glUseProgram(program);
    float amount = transforms[PARTICLE::SMOKE].size();
    applyBindings(program, projection, PARTICLE::SMOKE);

    glActiveTexture(GL_TEXTURE0);
    GLuint texture_id = renderer->texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::SMOKE];
    glBindTexture(GL_TEXTURE_2D, texture_id);

    GLuint opacity_loc = glGetUniformLocation(program, "opacity");
    glUniform1f(opacity_loc, 0.1);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr, amount);
    gl_has_errors();
}

void ParticleSystem::drawDash(const GLuint program, mat3& projection)
{
    glUseProgram(program);
    float amount = transforms[PARTICLE::DASH].size();
    applyBindings(program, projection, PARTICLE::DASH);

    glActiveTexture(GL_TEXTURE0);
    GLuint texture_id = renderer->texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::JEFF_JUMP];
    glBindTexture(GL_TEXTURE_2D, texture_id);

    GLuint opacity_loc = glGetUniformLocation(program, "opacity");
    glUniform1f(opacity_loc, 0.2);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr, amount);
    gl_has_errors();
}

Entity ParticleSystem::createSmokeParticle(vec3 position, vec2 size)
{
    Entity entity = Entity();

    Particle& particle = registry.particles.emplace(entity);
    particle.position = position;
    particle.velocity = vec3((uniform_dist(rng) - 0.5) / 10, (uniform_dist(rng) - 0.5) / 10, 0.1);
    particle.scale = size;
    particle.gravity = 0;
    particle.life = 1000;
    particle.type = PARTICLE::SMOKE;

    return entity;
}

Entity ParticleSystem::createDashParticle(vec3 position, vec2 size)
{
    Entity entity = Entity();

    Particle& particle = registry.particles.emplace(entity);
    particle.position = position;
    particle.velocity = vec3(0);
    particle.scale = size;
    particle.gravity = 0;
    particle.life = 100;
    particle.type = PARTICLE::DASH;

    return entity;
}
