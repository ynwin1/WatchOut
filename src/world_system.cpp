#include "world_system.hpp"
#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_init.hpp"
#include "physics_system.hpp"
#include <iostream>


WorldSystem::WorldSystem():
    spawn_functions({
        {"boar", createBoar},
        {"barbarian", createBarbarian},
        {"archer", createArcher}
    }),
    spawn_delays({ 
        {"boar", 3000},
        {"barbarian", 5000},
        {"archer", 10000}
    }),
    max_entities({
        {"boar", 2},
        {"barbarian", 5},
        {"archer", 1}
    })
{
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

void WorldSystem::init(RenderSystem* renderer, GLFWwindow* window, Camera* camera, PhysicsSystem* physics)
{
	this->renderer = renderer;
	this->window = window;
  this->camera = camera;
  this->physics = physics;

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);

    restart_game();
}

WorldSystem::~WorldSystem() {
	// Destroy all created components
	registry.clear_all_components();
}

void WorldSystem::restart_game()
{
    entity_types = {
        "barbarian",
        "boar",
        "archer"
    };

    // Create player entity
    playerEntity = createJeff(renderer, vec2(world_size_x / 2.f, world_size_y / 2.f));

    next_spawns = spawn_delays;
}

bool WorldSystem::step(float elapsed_ms)
{
    spawn(elapsed_ms);
    update_positions(elapsed_ms);
    update_cooldown(elapsed_ms);
    
    if(camera->isToggled()) {
        Motion& playerMotion = registry.motions.get(playerEntity);
        camera->followPosition(playerMotion.position);
    }

    think();
	return !is_over();
}

void WorldSystem::handle_collisions()
{
    std::vector<Entity> was_damaged;
	// Loop over all collisions detected by the physics system
	for (uint i = 0; i < physics->collisions.size(); i++) {
		// The entity and its collider
		Entity entity = physics->collisions[i].first;
		Entity entity_other = physics->collisions[i].second;
		
		// If the entity is a player
		if (registry.players.has(entity)) {
			// If the entity is colliding with a collectible
			if (registry.collectibles.has(entity_other)) {
				// destroy the collectible
				registry.remove_all_components_of(entity_other);

				// increase collectible count in player
				Player& player = registry.players.get(entity);
				player.trapsCollected++;
				printf("Player collected a trap\n");
			}
			else if (registry.traps.has(entity_other)) {
				printf("Player hit a trap\n");

				// reduce player health
				Player& player = registry.players.get(entity);
				Trap& trap = registry.traps.get(entity_other);
				int new_health = player.health - trap.damage;
				player.health = new_health < 0 ? 0 : new_health;
                was_damaged.push_back(entity);
				printf("Player health reduced by trap from %d to %d\n", player.health + trap.damage, player.health);

                // destroy the trap
                registry.remove_all_components_of(entity_other);
                // TODO LATER - Logic to handle player death
            }
			// Enemy attacks the player while it can (no cooldown)
			else if (registry.enemies.has(entity_other) && !registry.cooldowns.has(entity_other)) {
				// player takes the damage
				Player& player = registry.players.get(entity);
                Enemy& enemy = registry.enemies.get(entity_other);

                // Calculate potential new health
                int new_health = player.health - enemy.damage;
                player.health = new_health < 0 ? 0 : new_health;
                was_damaged.push_back(entity);
                printf("Player health reduced by enemy from %d to %d\n", player.health + enemy.damage, player.health);

				// Set cooldown for the enemy
				Cooldown& cooldown = registry.cooldowns.emplace(entity_other);
				cooldown.remaining = enemy.cooldown;
                
                // TODO LATER - Logic to handle player death
				// TODO M1 [WO-13] - Change player color to (red) for a short duration
			}
		}
		else if (registry.enemies.has(entity)) {
			if (registry.traps.has(entity_other)) {
				printf("Enemy hit a trap\n");

				// reduce enemy health
				Enemy& enemy = registry.enemies.get(entity);
				Trap& trap = registry.traps.get(entity_other);
				
				int new_health = enemy.health - trap.damage;
				enemy.health = new_health < 0 ? 0 : new_health;
                was_damaged.push_back(entity);
				printf("Enemy health reduced from %d to %d\n", enemy.health + trap.damage, enemy.health);
                
                // destroy the trap
                registry.remove_all_components_of(entity_other);

                // TODO LATER - Logic to handle enemy death
			}
			else if (registry.enemies.has(entity_other)) {
				// Reduce health of both enemies
				Enemy& enemy1 = registry.enemies.get(entity);
				Enemy& enemy2 = registry.enemies.get(entity_other);

                auto& allCooldowns = registry.cooldowns;
				// enemy 1 can attack enemy 2
                if (!allCooldowns.has(entity)) {
                    int newE2Health = enemy2.health - enemy1.damage;
                    enemy2.health = newE2Health < 0 ? 0 : newE2Health;
                    was_damaged.push_back(entity_other);
                    printf("Enemy %d's health reduced from %d to %d\n", (unsigned int)entity_other, enemy2.health + enemy1.damage, enemy2.health);

					// Set cooldown for enemy 1 
					Cooldown& cooldown = registry.cooldowns.emplace(entity);
					cooldown.remaining = enemy1.cooldown;

                }
				// enemy 2 can attack enemy 1
                if (!allCooldowns.has(entity_other)) {
					int newE1Health = enemy1.health - enemy2.damage;
					enemy1.health = newE1Health < 0 ? 0 : newE1Health;
                    was_damaged.push_back(entity);
					printf("Enemy %d's health reduced from %d to %d\n", (unsigned int)entity, enemy1.health + enemy2.damage, enemy1.health);

					// Set cooldown for enemy 2
					Cooldown& cooldown = registry.cooldowns.emplace(entity_other);
					cooldown.remaining = enemy2.cooldown;
          }

          // TODO LATER - Logic to handle enemy deaths
			}
		}
	}
  
	// Clear all collisions
    renderer->turn_damaged_red(was_damaged);
	physics->collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
    
}

void WorldSystem::on_key(int key, int, int action, int mod)
{
	Player& player_comp = registry.players.get(playerEntity);
    Motion& player_motion = registry.motions.get(playerEntity);
    Dash& player_dash = registry.dashers.get(playerEntity);

	if (action == GLFW_PRESS && key == GLFW_KEY_ENTER) {
        // TODO LATER - Think about where exactly to place the trap
        // Currently, it is placed at the player's position
        // MAYBE - Place it behind the player in the direction they are facin
        
        // Player position
        vec2 playerPos = player_motion.position;
        // Reduce player's trap count
        if (player_comp.trapsCollected == 0) {
            printf("Player has no traps to place\n");
            // TODO LATER - Do something to indicate that player has no traps [Milestone AFTER]
            return;
        }

        // Create a trap at player's position
        createDamageTrap(renderer, playerPos);
        player_comp.trapsCollected--;
        printf("Trap placed at (%f, %f)\n", playerPos.x, playerPos.y);
	}

    // Check key actions (press/release)
    if (action == GLFW_PRESS || action == GLFW_RELEASE)
    {
        bool pressed = (action == GLFW_PRESS);
        
        // Set movement states based on key input
        switch (key)
        {
            case GLFW_KEY_W:
                player_comp.goingUp = pressed;
                break;
            case GLFW_KEY_S:
                player_comp.goingDown = pressed;
                break;
            case GLFW_KEY_A:
                player_comp.goingLeft = pressed;
                break;
            case GLFW_KEY_D:
                player_comp.goingRight = pressed;
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
                if (pressed) {
                    const float dashDistance = 300;
                    // Start dashing if player is moving
                    player_dash.isDashing = true;
                    player_dash.dashStartPosition = player_motion.position;
                    player_dash.dashTargetPosition = player_motion.position + player_comp.facing * dashDistance;
                    player_dash.dashTimer = 0.0f; // Reset timer
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
    
    update_player_facing(player_comp);

    // toggle camera on/off for debugging/testing
    if(action == GLFW_PRESS && key == GLFW_KEY_C) {
        if(camera->toggle()) {
		    glfwSetWindowSize(window, camera->getWidth(), camera->getHeight());
	    } else {
		    glfwSetWindowSize(window, world_size_x, world_size_y);
	    }
    }
}

void WorldSystem::update_player_facing(Player& player) 
{
    vec2 player_facing = { 
        player.goingRight - player.goingLeft,
        player.goingDown - player.goingUp
    };

    // Keep old facing direction if no direction keys are pressed
    if (player_facing == vec2(0, 0)) {
        player.isMoving = false;
    }
    else {
        player.isMoving = true;
        player.facing = normalize(player_facing);
    }
}

void WorldSystem::update_positions(float elapsed_ms)
{

    for (Entity entity : registry.motions.entities) {
        float Running_Speed = 1.0f;
        // Get the Motion component of the entity
        Motion& motion = registry.motions.get(entity);
		Hitbox& hitbox = registry.hitboxes.get(entity);

        if(registry.players.has(entity)){
            Player& player_comp = registry.players.get(entity);

            float player_speed = 0.5;
            if (!player_comp.isMoving) player_speed = 0;
            else if (player_comp.isRunning) player_speed *= 2;

            motion.velocity = player_speed * player_comp.facing;
        }

        // Update the entity's position based on its velocity and elapsed time
        motion.position.x += motion.velocity.x * elapsed_ms;
        motion.position.y += motion.velocity.y * elapsed_ms;


        // Dashing overwrites normal movement
        if (registry.dashers.has(entity)){
            Dash& dashing = registry.dashers.get(entity);
            if (dashing.isDashing) {
                dashing.dashTimer += elapsed_ms / 1000.0f; // Converting ms to seconds

                if (dashing.dashTimer < dashing.dashDuration) {
                    // Interpolation factor
                    float t = dashing.dashTimer / dashing.dashDuration;

                    // Interpolate between start and target positions
                    //player_motion.position is the target_position for the linear interpolation formula L(t)=(1−t)⋅A+t⋅B
		            // L(t) = interpolated position, A = original position, B = target position, and t is the interpolation factor
                    motion.position = glm::mix(dashing.dashStartPosition, dashing.dashTargetPosition, t);
                } else {
                    motion.position = dashing.dashTargetPosition;
                    dashing.isDashing = false; // Reset isDashing
                }
            }  
        }    
       
		// Update the hitbox position
		hitbox.position = motion.position;
    }
    

}

void WorldSystem::update_cooldown(float elapsed_ms) {
    for (auto& cooldownEntity : registry.cooldowns.entities) {
		Cooldown& cooldown = registry.cooldowns.get(cooldownEntity);
		float new_remaining = cooldown.remaining - elapsed_ms;
		cooldown.remaining = new_remaining < 0 ? 0 : new_remaining;

		// Avaialble to attack again
        if (cooldown.remaining == 0) {
            registry.cooldowns.remove(cooldownEntity);
        }
    }
}

void WorldSystem::spawn(float elapsed_ms)
{
    for (std::string& entity_type : entity_types) {
        next_spawns.at(entity_type) -= elapsed_ms;
        if (next_spawns.at(entity_type) < 0 && registry.spawnable_lists.at(entity_type)->size() < max_entities.at(entity_type)) {
            vec2 spawnLocation = { 100, 100 };
            spawn_func f = spawn_functions.at(entity_type);
            (*f)(renderer, spawnLocation);
            next_spawns[entity_type] = spawn_delays.at(entity_type);
        }
    }
}


void WorldSystem::think()
{
    Motion& playerMotion = registry.motions.get(playerEntity);

    const float BOAR_SPEED = 0.4;
    for (Entity boar : registry.boars.entities) {
        Motion& enemyMotion = registry.motions.get(boar);
        vec2 direction = playerMotion.position - enemyMotion.position;
        direction /= distance(playerMotion.position, enemyMotion.position);
        enemyMotion.velocity = direction * BOAR_SPEED;
    }

    const float BARBARIAN_SPEED = 0.3;
    for (Entity enemy : registry.barbarians.entities) {
        Motion& enemyMotion = registry.motions.get(enemy);
        vec2 direction = playerMotion.position - enemyMotion.position;
        direction /= distance(playerMotion.position, enemyMotion.position);
        enemyMotion.velocity = direction * BARBARIAN_SPEED;
    }

    const float ARCHER_SPEED = 0.2;
    for (Entity archer : registry.archers.entities) {
        Motion& enemyMotion = registry.motions.get(archer);
        vec2 direction = playerMotion.position - enemyMotion.position;
        direction /= distance(playerMotion.position, enemyMotion.position);
        enemyMotion.velocity = direction * ARCHER_SPEED;
    }
}
