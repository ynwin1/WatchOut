#include <common.hpp>
#include <vector_float2.hpp>
#include <vector_float4.hpp>
#include "components.hpp"
#include "render_components.hpp"
// Particle system based off LearnOpenGl Tutorial: https://learnopengl.com/In-Practice/2D-Game/Particles
struct Particle {
    Motion motion;
    glm::vec4 Color;
    float     Life;
  
    Particle() : motion(), Color(1.0f), Life(0.0f) {}
};

// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator
{
public:
    // constructor
    ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);
    // update all particles
    void Update(float dt, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    // render all particles
    void Draw();
private:
    // state
    std::vector<Particle> particles;
    unsigned int amount;
    // render state
    TEXTURE_ASSET_ID textureID;
    unsigned int VAO;
    // initializes buffer and vertex attributes
    void init();
    // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    unsigned int firstUnusedParticle();
    // respawns particle
    void respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};