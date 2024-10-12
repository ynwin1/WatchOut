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
        {"barbarian", 8000},
        {"archer", 10000}
        }),
    max_entities({
        {"boar", 2},
        {"barbarian", 2},
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
    createBattleGround();
    entity_types = {
        "barbarian",
        "boar",
        "archer"
    };

    // Create player entity
    playerEntity = createJeff(renderer, vec2(world_size_x / 2.f, world_size_y / 2.f));
    game_over = false;
    is_paused = false;

    next_spawns = spawn_delays;
}

bool WorldSystem::step(float elapsed_ms)
{
    spawn(elapsed_ms);
    update_positions(elapsed_ms);
    update_cooldown(elapsed_ms);
    update_hp_positions();
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
				entity_collectible_collision(entity, entity_other);
            }
            else if (registry.traps.has(entity_other)) {
				// Collision between player and trap
				entity_trap_collision(entity, entity_other, was_damaged);
            }
            else if (registry.enemies.has(entity_other)) {
				// Collision between player and enemy
				moving_entities_collision(entity, entity_other, was_damaged);
            }
        }
        else if (registry.enemies.has(entity)) {
            if (registry.traps.has(entity_other)) {
				// Collision between enemy and trap
				entity_trap_collision(entity, entity_other, was_damaged);
            }
            else if (registry.enemies.has(entity_other)) {
				// Collision between two enemies
				moving_entities_collision(entity, entity_other, was_damaged);
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

    // Handle EP to pause gameplay
    if (action == GLFW_PRESS && key == GLFW_KEY_P) {
        if(is_paused == false){
            is_paused = true;
        } else{
            is_paused = false;
        }
        
    }

    // Check key actions (press/release)
    if (action == GLFW_PRESS || action == GLFW_RELEASE)
    {
        bool pressed = (action == GLFW_PRESS);

        // Set movement states based on key input
        switch (key)
        {
            case GLFW_KEY_W:
            case GLFW_KEY_UP:
                player_comp.goingUp = pressed;
                break;
            case GLFW_KEY_S:
            case GLFW_KEY_DOWN:
                player_comp.goingDown = pressed;
                break;
            case GLFW_KEY_A:
            case GLFW_KEY_LEFT:
                player_comp.goingLeft = pressed;
                break;
            case GLFW_KEY_D:
            case GLFW_KEY_RIGHT:
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
        camera->toggle();
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
            vec2 spawnLocation = get_spawn_location(entity_type);
            spawn_func f = spawn_functions.at(entity_type);
            (*f)(renderer, spawnLocation);
            next_spawns[entity_type] = spawn_delays.at(entity_type);
        }
    }
}

vec2 WorldSystem::get_spawn_location(const std::string& entity_type)
{
    int side = (int)(uniform_dist(rng) * 4);
    float loc = uniform_dist(rng);
    vec2 size = entity_sizes.at(entity_type);
    vec2 spawn_location{};
    if (side == 0) {
        // Spawn north
        spawn_location.x = size.x / 2.f + (world_size_x - size.x / 2.f) * loc;
        spawn_location.y = size.y / 2.f;
    }
    else if (side == 1) {
        // Spawn south
        spawn_location.x = size.x / 2.f + (world_size_x - size.x / 2.f) * loc;
        spawn_location.y = world_size_y - (size.y / 2.f);
    }
    else if (side == 2) {
        // Spawn west
        spawn_location.x = size.x / 2.f;
        spawn_location.y = size.y / 2.f + (world_size_y - size.y / 2.f) * loc;
    }
    else {
        // Spawn east
        spawn_location.x = world_size_x - (size.x / 2.f);
        spawn_location.y = size.y / 2.f + (world_size_y - size.y / 2.f) * loc;
    }
    return spawn_location;
}


void WorldSystem::think()
{
    Motion& playerMotion = registry.motions.get(playerEntity);

    const float BOAR_SPEED = 0.4;
    for (Entity boar : registry.boars.entities) {
		if (registry.deathTimers.has(boar)) continue; // Skip dead boars
        Motion& enemyMotion = registry.motions.get(boar);
        vec2 direction = normalize(playerMotion.position - enemyMotion.position);
        enemyMotion.velocity = direction * BOAR_SPEED;
    }

    const float BARBARIAN_SPEED = 0.3;
    for (Entity enemy : registry.barbarians.entities) {
		if (registry.deathTimers.has(enemy)) continue; // Skip dead barbarians
        Motion& enemyMotion = registry.motions.get(enemy);
        vec2 direction = normalize(playerMotion.position - enemyMotion.position);
        enemyMotion.velocity = direction * BARBARIAN_SPEED;
    }

    const float ARCHER_SPEED = 0.2;
    for (Entity archer : registry.archers.entities) {
		if (registry.deathTimers.has(archer)) continue; // Skip dead archers
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
    const float RECOIL_STRENGTH = 0.3;
    if (y_overlap < x_overlap) {
        motion1.position.y += y_direction * y_overlap * RECOIL_STRENGTH;
        motion2.position.y -= y_direction * y_overlap * RECOIL_STRENGTH;
    }
    else {
        motion1.position.x += x_direction * x_overlap * RECOIL_STRENGTH;
        motion2.position.x -= x_direction * x_overlap * RECOIL_STRENGTH;
    }
    
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

// Collision functions
void WorldSystem::entity_collectible_collision(Entity entity, Entity entity_other) {
    // ONLY PLAYER CAN COLLECT COLLECTIBLES
    
    // destroy the collectible
    registry.remove_all_components_of(entity_other);

    // increase collectible count in player
    Player& player = registry.players.get(entity);
    player.trapsCollected++;
    printf("Player collected a trap\n");
}

void WorldSystem::entity_trap_collision(Entity entity, Entity entity_other, std::vector<Entity>& was_damaged) {
    Trap& trap = registry.traps.get(entity_other);

    if (registry.players.has(entity)) {
        printf("Player hit a trap\n");

        // reduce player health
        Player& player = registry.players.get(entity);
        int new_health = player.health - trap.damage;
        player.health = new_health < 0 ? 0 : new_health;
		was_damaged.push_back(entity);
        printf("Player health reduced by trap from %d to %d\n", player.health + trap.damage, player.health);
	}
	else if (registry.enemies.has(entity)) {
        printf("Enemy hit a trap\n");

        // reduce enemy health
        Enemy& enemy = registry.enemies.get(entity);
        int new_health = enemy.health - trap.damage;
        enemy.health = new_health < 0 ? 0 : new_health;
        was_damaged.push_back(entity);
        printf("Enemy health reduced from %d to %d\n", enemy.health + trap.damage, enemy.health);
	}
	else {
		printf("Entity is not a player or enemy\n");
		return;
	}

    // destroy the trap
    registry.remove_all_components_of(entity_other);
}

void WorldSystem::moving_entities_collision(Entity entity, Entity entityOther, std::vector<Entity>& was_damaged) {
    if (registry.players.has(entity)) {
        processPlayerEnemyCollision(entity, entityOther, was_damaged);
    }
    else if (registry.enemies.has(entity)) {
        processEnemyEnemyCollision(entity, entityOther, was_damaged);
    }
}

void WorldSystem::processPlayerEnemyCollision(Entity player, Entity enemy, std::vector<Entity>& was_damaged) {
    Motion& playerMotion = registry.motions.get(player);
    Motion& enemyMotion = registry.motions.get(enemy);
    recoil_entities(playerMotion, enemyMotion);

    if (!registry.cooldowns.has(enemy)) {
        Player& playerData = registry.players.get(player);
        Enemy& enemyData = registry.enemies.get(enemy);

        int newHealth = playerData.health - enemyData.damage;
        playerData.health = std::max(newHealth, 0);
        was_damaged.push_back(player);
        printf("Player health reduced by enemy from %d to %d\n", playerData.health + enemyData.damage, playerData.health);

        Cooldown& cooldown = registry.cooldowns.emplace(enemy);
        cooldown.remaining = enemyData.cooldown;

        if (playerData.health == 0) {
            playerMotion.angle = 1.57f; // Rotate player 90 degrees
            printf("Player died\n");
        }
    }
}

void WorldSystem::processEnemyEnemyCollision(Entity enemy1, Entity enemy2, std::vector<Entity>& was_damaged) {
    Motion& motion1 = registry.motions.get(enemy1);
    Motion& motion2 = registry.motions.get(enemy2);
    recoil_entities(motion1, motion2);

    handleEnemyCollision(enemy1, enemy2, was_damaged);
    handleEnemyCollision(enemy2, enemy1, was_damaged);

    checkAndHandleEnemyDeath(enemy1);
    checkAndHandleEnemyDeath(enemy2);
}

void WorldSystem::handleEnemyCollision(Entity attacker, Entity target, std::vector<Entity>& was_damaged) {
    if (!registry.cooldowns.has(attacker)) {
        Enemy& attackerData = registry.enemies.get(attacker);
        Enemy& targetData = registry.enemies.get(target);

        int newHealth = targetData.health - attackerData.damage;
        targetData.health = std::max(newHealth, 0);
        was_damaged.push_back(target);
        printf("Enemy %d's health reduced from %d to %d\n", (unsigned int)target, targetData.health + attackerData.damage, targetData.health);

        Cooldown& cooldown = registry.cooldowns.emplace(attacker);
        cooldown.remaining = attackerData.cooldown;
    }
}

void WorldSystem::checkAndHandleEnemyDeath(Entity enemy) {
    Enemy& enemyData = registry.enemies.get(enemy);
    if (enemyData.health == 0 && !registry.deathTimers.has(enemy)) {
        Motion& motion = registry.motions.get(enemy);
        motion.velocity = { 0, 0 }; // Stop enemy movement
        motion.angle = 1.57f; // Rotate enemy 90 degrees
        printf("Enemy %d died with health %d\n", (unsigned int)enemy, enemyData.health);

        HealthBar& hpbar = registry.healthBars.get(enemy);
        registry.remove_all_components_of(hpbar.meshEntity);
        registry.enemies.remove(enemy);
        registry.deathTimers.emplace(enemy);
    }
}

void hpBarPositionHelper(const std::vector<Entity>& entities) {
    for (Entity entity : entities) {
	    HealthBar& healthBar = registry.healthBars.get(entity);
        Motion& motion = registry.motions.get(entity);
        StaticMotion& healthBarMotion =  registry.staticMotions.get(healthBar.meshEntity);
        float halfScaleX = motion.scale.x / 2;
        float halfScaleY = motion.scale.y / 2;

        // place above character
        healthBarMotion.position.y = motion.position.y - halfScaleY - 18;
        healthBarMotion.position.x = motion.position.x - halfScaleX;
    }   
}

void WorldSystem::update_hp_positions() {
    hpBarPositionHelper(registry.players.entities);
    hpBarPositionHelper(registry.enemies.entities);
}


