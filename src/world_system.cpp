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

	createJeff(renderer, vec2(100, 100));
	// Create player entity
    playerEntity = createJeff(renderer, vec2(100, 100));

    // Add player component to registry (set initial values)
    registry.players.emplace(playerEntity, Player{ 100, false, false, false, false});
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
		//target_position for the linear interpolation formula
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
