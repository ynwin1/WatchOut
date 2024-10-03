#include "world_system.hpp"
#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_init.hpp"


WorldSystem::WorldSystem() {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

void WorldSystem::init(RenderSystem* renderer, GLFWwindow* window)
{
	this->renderer = renderer;
	this->window = window;

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);

	// Create player entity
    playerEntity = createJeff(renderer, vec2(100, 100));

}

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
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;
		
		// If the entity is a player
		if (registry.players.has(entity)) {
			// If the entity is colliding with a collectible
			if (registry.collectibles.has(entity_other)) {
				// TODO: [WO-17] Logic to collect the collectibles

			}
			else if (registry.traps.has(entity_other)) {
				// destroy the trap
				registry.remove_all_components_of(entity_other);
				printf("Player hit a trap\n");

				// reduce player health
				Player& player = registry.players.get(entity);
				Trap& trap = registry.traps.get(entity_other);
				player.health -= trap.damage;
				printf("Player health reduced by trap from %d to %d\n", player.health + trap.damage, player.health);
			}
			else if (registry.enemies.has(entity_other)) {
				// player takes the damage
				Player& player = registry.players.get(entity);
				Enemy& enemy = registry.enemies.get(entity_other);
				player.health -= enemy.damage;
				printf("Player health reduced by enemy from %d to %d\n", player.health + enemy.damage, player.health);

				// TODO M1 [WO-13] - Change player color to (red) for a short duration
			}
		}
		else if (registry.enemies.has(entity)) {
			if (registry.traps.has(entity_other)) {
				// destroy the trap
				registry.remove_all_components_of(entity_other);
				printf("Enemy hit a trap\n");

				// reduce enemy health
				Enemy& enemy = registry.enemies.get(entity);
				Trap& trap = registry.traps.get(entity_other);
				enemy.health -= trap.damage;
				printf("Enemy health reduced from %d to %d\n", enemy.health + trap.damage, enemy.health);
			}
			else if (registry.enemies.has(entity_other)) {
				// TODO LATER - Logic for enemy-enemy collision
			}
		}
	}
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}
void WorldSystem::on_mouse_move(vec2 mouse_position){
    
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
                if (pressed && player_motion.velocity != glm::vec2(0.0f, 0.0f)) {
                    // Start dashing if player is moving
                    player_comp.isDashing = true;
                    player_comp.dashStartPosition = player_motion.position;

                    // Calculate dash target position based on current velocity
                    glm::vec2 dashDirection = glm::normalize(player_motion.velocity);
                    const float dashDistance = 500.0f; 
                    player_comp.dashTargetPosition = player_motion.position + dashDirection * dashDistance;
                    player_comp.dashTimer = 0.0f; // Reset timer
                }
                break;
				case GLFW_KEY_SPACE:
                // Dash
                player_comp.isJumping = pressed;
                break;
            default:
                break;
        }
    }

    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_W || key == GLFW_KEY_S) {
            player_motion.velocity.y = 0.f; // Stop vertical movement
        }
        if (key == GLFW_KEY_A || key == GLFW_KEY_D) {
            player_motion.velocity.x = 0.f; // Stop horizontal movement
        }
    }
}

void WorldSystem::update_player_movement(float elapsed_ms)
{
    Player& player_comp = registry.players.get(playerEntity);
    Motion& player_motion = registry.motions.get(playerEntity);
    const float dashDuration = 0.2f;

	// Determine speed based on whether the player is running
    const float speed = player_comp.isRunning ? 2.0f : 1.0f;

    // Update the player's position based on velocity and elapsed time
    player_motion.position.x += player_motion.velocity.x * speed * elapsed_ms;
    player_motion.position.y += player_motion.velocity.y * speed * elapsed_ms;

    if (player_comp.isDashing) {
        player_comp.dashTimer += elapsed_ms / 1000.0f; // Converting ms to seconds

        if (player_comp.dashTimer < dashDuration) {
            // Interpolation factor
            float t = player_comp.dashTimer / dashDuration;

        // Interpolate between start and target positions
        //player_motion.position is the target_position for the linear interpolation formula L(t)=(1−t)⋅A+t⋅B
		// L(t) = interpolated position, A = original position, B = target position, and t is the interpolation factor
            player_motion.position = glm::mix(player_comp.dashStartPosition, player_comp.dashTargetPosition, t);
        } else {
            player_motion.position = player_comp.dashTargetPosition;
            player_comp.isDashing = false; // Reset isDashing
        }
    }
    

}

void WorldSystem::restart_game()
{
}
