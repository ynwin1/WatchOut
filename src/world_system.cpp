#include "world_system.hpp"
#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_init.hpp"
#include "physics_system.hpp"
#include <iostream>


WorldSystem::WorldSystem() :
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
        {"boar", 1},
        {"barbarian", 1},
        {"archer", 0}
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
    registry.clear_all_components();
    entity_types = {
        "barbarian",
        "boar",
        "archer"
    };

    // Create player entity
    playerEntity = createJeff(renderer, vec2(world_size_x / 2.f, world_size_y / 2.f));
    game_over = false;

    next_spawns = spawn_delays;
}

bool WorldSystem::step(float elapsed_ms)
{
    spawn(elapsed_ms);
    update_positions(elapsed_ms);
    update_cooldown(elapsed_ms);
    handle_deaths(elapsed_ms);

    if (camera->isToggled()) {
        Motion& playerMotion = registry.motions.get(playerEntity);
        camera->followPosition(playerMotion.position);
    }

    Player& player = registry.players.get(playerEntity);
    if(player.health == 0) {
        //CREATE GAMEOVER ENTITY
        vec2 camera_pos = camera->getPosition();
        createGameOver(renderer, camera_pos);
        game_over = true;
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
            else if (registry.enemies.has(entity_other)) {

				// Recoil the entities
				Motion& playerMotion = registry.motions.get(entity);
				Motion& enemyMotion = registry.motions.get(entity_other);
				recoil_entities(playerMotion, enemyMotion);

                if (!registry.cooldowns.has(entity_other)) {
                    // player takes the damage
                    Player& player = registry.players.get(entity);
                    Enemy& enemy = registry.enemies.get(entity_other);

                    // Calculate potential new health
                    int new_health = player.health - enemy.damage;
                    player.health = new_health <= 0 ? 0 : new_health;
                    was_damaged.push_back(entity);
                    printf("Player health reduced by enemy from %d to %d\n", player.health + enemy.damage, player.health);

                    // Set cooldown for the enemy
                    Cooldown& cooldown = registry.cooldowns.emplace(entity_other);
                    cooldown.remaining = enemy.cooldown;

                    // Handle player death
                    if (player.health == 0) {
                        Motion& playerMotion = registry.motions.get(entity);
                        playerMotion.angle = 1.57f; // Rotate player 90 degrees
                        printf("Player died\n");
                    }
                }
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

				// Recoil the enemies
				Motion& enemyMotion1 = registry.motions.get(entity);
				Motion& enemyMotion2 = registry.motions.get(entity_other);
				recoil_entities(enemyMotion1, enemyMotion2);

                auto& allCooldowns = registry.cooldowns;
                // enemy 1 can attack enemy 2
                if (!allCooldowns.has(entity)) {
                    int newE2Health = enemy2.health - enemy1.damage;
                    enemy2.health = newE2Health <= 0 ? 0 : newE2Health;
                    was_damaged.push_back(entity_other);
                    printf("Enemy %d's health reduced from %d to %d\n", (unsigned int)entity_other, enemy2.health + enemy1.damage, enemy2.health);

                    // Set cooldown for enemy 1 
                    Cooldown& cooldown = registry.cooldowns.emplace(entity);
                    cooldown.remaining = enemy1.cooldown;
                }
                // enemy 2 can attack enemy 1
                if (!allCooldowns.has(entity_other)) {
                    int newE1Health = enemy1.health - enemy2.damage;
                    enemy1.health = newE1Health <= 0 ? 0 : newE1Health;
                    was_damaged.push_back(entity);
                    printf("Enemy %d's health reduced from %d to %d\n", (unsigned int)entity, enemy1.health + enemy2.damage, enemy1.health);

                    // Set cooldown for enemy 2
                    Cooldown& cooldown = registry.cooldowns.emplace(entity_other);
                    cooldown.remaining = enemy2.cooldown;
                }

                // Handle enemy death
                if (enemy1.health == 0 && !registry.deathTimers.has(entity)) {
                    Motion& enemyMotion = registry.motions.get(entity);
                    enemyMotion.velocity = { 0, 0 }; // Stop enemy movement
                    enemyMotion.angle = 1.57f; // Rotate enemy 90 degrees
                    printf("Enemy %d died with health %d\n", (unsigned int)entity, enemy1.health);

                    // remove enemy from enemy
                    registry.enemies.remove(entity);
                    registry.deathTimers.emplace(entity);
                }
                if (enemy2.health == 0 && !registry.deathTimers.has(entity_other)) {
                    Motion& enemyMotion = registry.motions.get(entity_other);
                    enemyMotion.velocity = { 0, 0 }; // Stop enemy movement
                    enemyMotion.angle = 1.57f; // Rotate enemy 90 degrees
                    printf("Enemy %d died with health %d\n", (unsigned int)entity_other, enemy2.health);

                    registry.enemies.remove(entity_other);
                    registry.deathTimers.emplace(entity_other);
                }
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

    if (game_over) {
        if (action == GLFW_PRESS && key == GLFW_KEY_ENTER){
            restart_game();
            return;
        }
    }

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

    // Handle ESC key to close the game window
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
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
        // Primary monitor and its video mode
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

        // Make the window fullscreen by resizing and positioning it
        glfwSetWindowPos(window, 0, 0); 
        glfwSetWindowSize(window, mode->width, mode->height); 
        glfwMakeContextCurrent(window);
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

void WorldSystem::handle_deaths(float elapsed_ms) {
    for (auto& deathEntity : registry.deathTimers.entities) {
        DeathTimer& deathTimer = registry.deathTimers.get(deathEntity);
        deathTimer.timer -= elapsed_ms;
        if (deathTimer.timer < 0) {
            registry.remove_all_components_of(deathEntity);
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
        vec2 direction = normalize(playerMotion.position - enemyMotion.position);
        enemyMotion.velocity = direction * BOAR_SPEED;
    }

    const float BARBARIAN_SPEED = 0.3;
    for (Entity enemy : registry.barbarians.entities) {
        Motion& enemyMotion = registry.motions.get(enemy);
        vec2 direction = normalize(playerMotion.position - enemyMotion.position);
        enemyMotion.velocity = direction * BARBARIAN_SPEED;
    }

    const float ARCHER_SPEED = 0.2;
    for (Entity archer : registry.archers.entities) {
        Motion& enemyMotion = registry.motions.get(archer);
        vec2 direction = normalize(playerMotion.position - enemyMotion.position);
        enemyMotion.velocity = direction * ARCHER_SPEED;
    }
}

void WorldSystem::recoil_entities(Motion& motion1, Motion& motion2) {
    // Calculate x overlap
    float x_overlap = calculate_x_overlap(motion1, motion2);
	// Calculate y overlap
	float y_overlap = calculate_y_overlap(motion1, motion2);

    // Calculate the direction of the collision
    float x_direction = motion1.position.x < motion2.position.x ? -1 : 1;
    float y_direction = motion1.position.y < motion2.position.y ? -1 : 1;

    // Apply the recoil (direction * magnitude)
    const float RECOIL_STRENGTH = 0.08;
    motion1.position.x += x_direction * x_overlap * RECOIL_STRENGTH;
    motion1.position.y += y_direction * y_overlap * RECOIL_STRENGTH;
    motion2.position.x -= x_direction * x_overlap * RECOIL_STRENGTH;
    motion2.position.y -= y_direction * y_overlap * RECOIL_STRENGTH;
}

float WorldSystem::calculate_x_overlap(Motion& motion1, Motion& motion2) {
	float x1_half_scale = motion1.scale.x / 2;
	float x2_half_scale = motion2.scale.x / 2;

	// Determine the edges of the hitboxes for x
	float left1 = motion1.position.x - x1_half_scale;
	float right1 = motion1.position.x + x1_half_scale;
	float left2 = motion2.position.x - x2_half_scale;
	float right2 = motion2.position.x + x2_half_scale;

	// Calculate x overlap
	return max(0.f, min(right1, right2) - max(left1, left2));
}

float WorldSystem::calculate_y_overlap(Motion& motion1, Motion& motion2) {
	float y1_half_scale = motion1.scale.y / 2;
	float y2_half_scale = motion2.scale.y / 2;

	// Determine the edges of the hitboxes for y
	float top1 = motion1.position.y - y1_half_scale;
	float bottom1 = motion1.position.y + y1_half_scale;
	float top2 = motion2.position.y - y2_half_scale;
	float bottom2 = motion2.position.y + y2_half_scale;

	// Calculate y overlap
	return max(0.f, min(bottom1, bottom2) - max(top1, top2));
}
