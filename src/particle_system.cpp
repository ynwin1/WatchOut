#include "particle_system.hpp"
#include "render_system.hpp"
#include "world_init.hpp"

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::init(RenderSystem* renderer)
{
    this->renderer = renderer;

    //int index = 0;
    //for (int y = -10; y < 5; y += 2)
    //{
    //    for (int x = -10; x < 5; x += 2)
    //    {
    //        Transform transform;
    //        transform.translate(vec2(x, y) * 200.f + vec2(500, 500));
    //        transform.scale(vec2(100, 100));
    //        transforms[index++] = transform.mat;
    //    }
    //}
    glGenBuffers(1, &transforms_vbo);
}

void ParticleSystem::draw(const GLuint program)
{
    mat3 projection = renderer->createProjectionMatrix();

    size_t amount = registry.particles.size();

    transforms.clear();
    transforms.reserve(amount);

    for (Entity entity : registry.particles.entities) {
        Motion& motion = registry.motions.get(entity);

        Transform transform;
        printf("%f, %f, %f\n", motion.position.x, motion.position.y, motion.position.z);
        transform.translate(worldToVisual(motion.position));
        transform.rotate(motion.angle);
        transform.scale(motion.scale);
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
    GLuint texture_id = renderer->texture_gl_handles[(GLuint)TEXTURE_ASSET_ID::ROCK];
    glBindTexture(GL_TEXTURE_2D, texture_id);

    GLuint projection_loc = glGetUniformLocation(program, "projection");
    glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr, amount);
    gl_has_errors();
}

Entity ParticleSystem::createSmokeParticle(vec3 position)
{
    Entity entity = Entity();

    Particle& particle = registry.particles.emplace(entity);
    particle.texture = TEXTURE_ASSET_ID::ROCK;

    Motion& motion = registry.motions.emplace(entity);
    motion.position = position;
    motion.velocity = vec3(0, 0, 1);
    motion.solid = false;
    motion.scale = vec2(ROCK_BB_WIDTH, ROCK_BB_HEIGHT);

    return entity;
}
