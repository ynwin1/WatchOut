#include "world_system.hpp"
#include "tiny_ecs_registry.hpp"
#include "common.hpp"


WorldSystem::WorldSystem() {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

void WorldSystem::init(RenderSystem* renderer, GLFWwindow* window)
{
	this->renderer = renderer;
	this->window = window;

	// Create player entity
    playerEntity = Entity();

    // Add player component to registry (set initial values)
    registry.players.emplace(playerEntity, Player{false, false, false, false});
	registry.motions.emplace(playerEntity, Motion{vec2(0, 0), 0, vec2(0, 0), vec2(10, 10)});

}
//glm::vec2(0, 0), glm::vec2(0, 0), 

WorldSystem::~WorldSystem() {
	// Destroy all created components
	registry.clear_all_components();
}

bool WorldSystem::step(float elapsed_ms)
{
	update_player_movement(elapsed_ms);
	return !is_over();
}

void WorldSystem::handle_collisions()
{
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

void WorldSystem::on_key(int key, int, int action, int mod)
{
	Player& player_comp = registry.players.get(playerEntity);
	Motion& player_motion = registry.motions.get(playerEntity);

    // Check key actions (press/release)
    if (action == GLFW_PRESS || action == GLFW_RELEASE)
    {
        bool pressed = (action == GLFW_PRESS);
        
        // Set movement states based on key input
        switch (key)
        {
            case GLFW_KEY_W:
                // Set velocity upward
                player_motion.velocity.y = pressed ? -1.0f : 0.0f;
                break;
            case GLFW_KEY_S:
                // Set velocity downward
                player_motion.velocity.y = pressed ? 1.0f : 0.0f;
                break;
            case GLFW_KEY_A:
                // Set velocity left
                player_motion.velocity.x = pressed ? -1.0f : 0.0f;
                break;
            case GLFW_KEY_D:
                // Set velocity right
                player_motion.velocity.x = pressed ? 1.0f : 0.0f;
                break;
            case GLFW_KEY_LEFT_SHIFT:
                // Sprint
                player_comp.isRunning = pressed;
                break;
            case GLFW_KEY_R:
                // Roll
                player_comp.isRolling = pressed;
                break;
            case GLFW_KEY_F:
                // Dash
                player_comp.isDashing = pressed;
                break;
				case GLFW_KEY_SPACE:
                // Dash
                player_comp.isJumping = pressed;
                break;
            default:
                break;
        }
    }
}

void WorldSystem::update_player_movement(float elapsed_ms)
{
    Player& player_comp = registry.players.get(playerEntity);
	Motion& player_motion = registry.motions.get(playerEntity);

	// Determine speed based on whether the player is running
    const float speed = player_comp.isRunning ? 2.0f : 1.0f;

    // Update the player's position based on velocity and elapsed time
    player_motion.position.x += player_motion.velocity.x * speed * elapsed_ms;
    player_motion.position.y += player_motion.velocity.y * speed * elapsed_ms;

	if (player_comp.isDashing) {
        // (dash distance = 4 units (arbitrary can be changed))
		//target_position for the linear interpolation formula L(t)=(1−t)⋅A+t⋅B
		// L(t) = interpolated position, A = original position, B = target position, and t is the interpolation factor (or in our case determines how long we want the dash to last)
        glm::vec2 target_position = player_motion.position + glm::normalize(player_motion.velocity) * 4.0f; 

        // Interpolating position
        float t = elapsed_ms / 100.0f; // Arbitrary t (can be changed according to our requirements)
        player_motion.position = glm::mix(player_motion.position, target_position, t); // Using glm::mix for linear interpolation

        // Reset dashing state after dashing
        player_comp.isDashing = false;
    }
    

    // Reset velocity after applying movement
    player_motion.velocity = glm::vec2(0, 0);
}

void WorldSystem::restart_game()
{
}
