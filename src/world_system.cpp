#include "world_system.hpp"
#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_init.hpp"
#include "physics_system.hpp"
#include <iostream>
#include <iomanip> 
#include <sstream>

WorldSystem::WorldSystem() :
    spawn_functions({
        {"boar", createBoar},
        {"barbarian", createBarbarian},
        {"archer", createArcher},
        {"heart", createHeart},
		{"collectible_trap", createCollectibleTrap}
        }),
    spawn_delays({
        {"boar", 3000},
        {"barbarian", 8000},
        {"archer", 10000},
		{"heart", 15000},
		{"collectible_trap", 6000}
        }),
    max_entities({
        {"boar", 2},
        {"barbarian", 2},
        {"archer", 0},
		{"heart", 1},
		{"collectible_trap", 1}
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
        "archer",
        "heart",
        "collectible_trap"
    };

    // Create player entity
    playerEntity = createJeff(renderer, vec2(world_size_x / 2.f, world_size_y / 2.f));
    
    
    game_over = false;
    is_paused = false;

    initText();

    next_spawns = spawn_delays;
}

void WorldSystem::updateGameTimer(float elapsed_ms) {
    gameTimer.update(elapsed_ms);
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << gameTimer.hours << ":"
       << std::setw(2) << std::setfill('0') << gameTimer.minutes << ":"
       << std::setw(2) << std::setfill('0') << gameTimer.seconds;

    Text& text = registry.texts.get(gameTimer.textEntity);
    text.value = ss.str();
}

void WorldSystem::initText() {
    registry.fpsTracker.textEntity = createFPSText(camera->getSize());
    gameTimer.reset();
    gameTimer.textEntity = createGameTimerText(camera->getSize());
    trapsCounter.reset();
    trapsCounter.textEntity = createTrapsCounterText(camera->getSize());
}

void WorldSystem::trackFPS(float elapsed_ms) {
    FPSTracker& fpsTracker = registry.fpsTracker; 
    fpsTracker.update(elapsed_ms);

    if(fpsTracker.elapsedTime == 0) {
        Text& text = registry.texts.get(fpsTracker.textEntity);
        text.value = std::to_string(fpsTracker.fps) + " fps";
    }
}

void WorldSystem::updateTrapsCounterText() {
    Text& text = registry.texts.get(trapsCounter.textEntity);
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << trapsCounter.count;
    text.value = "Traps: " + ss.str();
}

bool WorldSystem::step(float elapsed_ms)
{
    spawn(elapsed_ms);
    update_cooldown(elapsed_ms);
    handle_deaths(elapsed_ms);
    despawn_collectibles(elapsed_ms);
    handle_stamina(elapsed_ms);
    trackFPS(elapsed_ms);
    updateGameTimer(elapsed_ms);
    updateTrapsCounterText();
    updateEntityFacing();

    if (camera->isToggled()) {
        Motion& playerMotion = registry.motions.get(playerEntity);
        camera->followPosition(getVisualPosition(playerMotion.position));
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
    Stamina& player_stamina = registry.staminas.get(playerEntity);

    if (game_over) {
        if (action == GLFW_PRESS && key == GLFW_KEY_ENTER){
            restart_game();
            return;
        }
    }

	if (action == GLFW_PRESS && key == GLFW_KEY_W) {
        place_trap(player_comp, player_motion, true);
	}

    if (action == GLFW_PRESS && key == GLFW_KEY_Q) {
        place_trap(player_comp, player_motion, false);
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
            case GLFW_KEY_UP:
                player_comp.goingUp = pressed;
                break;
            case GLFW_KEY_DOWN:
                player_comp.goingDown = pressed;
                break;
            case GLFW_KEY_LEFT:
                player_comp.goingLeft = pressed;
                break;
            case GLFW_KEY_RIGHT:
                player_comp.goingRight = pressed;
                break;
            case GLFW_KEY_LEFT_SHIFT:
                // Sprint
                if (player_stamina.stamina > 0) { 
                    player_comp.isRunning = pressed;
                } else{
                    player_comp.isRunning = false;
                }
                break;
            case GLFW_KEY_R:
                // Roll
                if (player_stamina.stamina > 0) { 
                    player_comp.isRolling = pressed;
                } else{
                    player_comp.isRolling = false;
                }
                break;
            case GLFW_KEY_D:
                if (pressed) {
                    if (player_stamina.stamina > 0) { 
                        const float dashDistance = 300;
                        // Start dashing if player is moving
                        player_dash.isDashing = true;
                        player_dash.dashStartPosition = vec2(player_motion.position);
                        player_dash.dashTargetPosition = player_dash.dashStartPosition + player_comp.facing * dashDistance;
                        player_dash.dashTimer = 0.0f; // Reset timer
                    }
                }
                break;
		    case GLFW_KEY_SPACE:
                // Jump
                if (pressed) {
                    const float min_stamina_for_jump = 10.0f;
                    if (player_stamina.stamina >= min_stamina_for_jump && !player_comp.tryingToJump) {
                        player_comp.tryingToJump = true;
                        if (registry.jumpers.has(playerEntity)) {
                            Jumper& jumper = registry.jumpers.get(playerEntity);
                            if (!jumper.isJumping) {  
                                jumper.isJumping = true; 
                                player_comp.tryingToJump = true;
                                player_motion.velocity.z = jumper.speed; 
                                player_stamina.stamina -= min_stamina_for_jump;
                                if (player_stamina.stamina < 0) {
                                    player_stamina.stamina = 0;
                                }
                            }
                        }
                    }
                } else { 
                    player_comp.tryingToJump = false;
                }
                //Handles player jumping too far up
                if (player_motion.velocity.z <= 0 && player_comp.tryingToJump) {
                    player_comp.tryingToJump = false;
                    if (registry.jumpers.has(playerEntity)) {
                        Jumper& jumper = registry.jumpers.get(playerEntity);
                        jumper.isJumping = false;
                    }
                }
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

    // toggle fps
    if(action == GLFW_PRESS && key == GLFW_KEY_F) {
        registry.fpsTracker.toggled = !registry.fpsTracker.toggled;
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

    // spawn heart
	if (entity_type == "heart" || entity_type == "collectible_trap") {
		// spawn at random location on the map
		spawn_location.x = loc * (world_size_x - (size.x + 10.0f) / 2.f);
		spawn_location.y = loc * (world_size_y - (size.y + 10.0f) / 2.f);
    }
    else 
	// spawn enemies
    {
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
        enemyMotion.velocity = vec3(direction * BOAR_SPEED, enemyMotion.velocity.z);
    }

    const float BARBARIAN_SPEED = 0.3;
    for (Entity enemy : registry.barbarians.entities) {
		if (registry.deathTimers.has(enemy)) continue; // Skip dead barbarians
        Motion& enemyMotion = registry.motions.get(enemy);
        vec2 direction = normalize(playerMotion.position - enemyMotion.position);
        enemyMotion.velocity = vec3(direction * BARBARIAN_SPEED, enemyMotion.velocity.z);
    }

    const float ARCHER_SPEED = 0.2;
    for (Entity archer : registry.archers.entities) {
		if (registry.deathTimers.has(archer)) continue; // Skip dead archers
        Motion& enemyMotion = registry.motions.get(archer);
        vec2 direction = normalize(playerMotion.position - enemyMotion.position);
        enemyMotion.velocity = vec3(direction * ARCHER_SPEED, enemyMotion.velocity.z);
    }
}

void WorldSystem::recoil_entities(Entity entity1, Entity entity2) {
    // Calculate x overlap
    float x_overlap = calculate_x_overlap(entity1, entity2);
	// Calculate y overlap
	float y_overlap = calculate_y_overlap(entity1, entity2);

    Motion& motion1 = registry.motions.get(entity1);
    Motion& motion2 = registry.motions.get(entity2);

    // Calculate the direction of the collision
    float x_direction = motion1.position.x < motion2.position.x ? -1 : 1;
    float y_direction = motion1.position.y < motion2.position.y ? -1 : 1;

    // Apply the recoil (direction * magnitude)
    const float RECOIL_STRENGTH = 0.25;
    if (y_overlap < x_overlap) {
        motion1.position.y += y_direction * y_overlap * RECOIL_STRENGTH;
        motion2.position.y -= y_direction * y_overlap * RECOIL_STRENGTH;
    }
    else {
        motion1.position.x += x_direction * x_overlap * RECOIL_STRENGTH;
        motion2.position.x -= x_direction * x_overlap * RECOIL_STRENGTH;
    }
    
}

float WorldSystem::calculate_x_overlap(Entity entity1, Entity entity2) {
    Hitbox& hitbox1 = registry.hitboxes.get(entity1);
    Hitbox& hitbox2 = registry.hitboxes.get(entity2);
    Motion& motion1 = registry.motions.get(entity1);
    Motion& motion2 = registry.motions.get(entity2);

	float x1_half_scale = hitbox1.dimension.x / 2;
	float x2_half_scale = hitbox2.dimension.x / 2;

	// Determine the edges of the hitboxes for x
	float left1 = motion1.position.x - x1_half_scale;
	float right1 = motion1.position.x + x1_half_scale;
	float left2 = motion2.position.x - x2_half_scale;
	float right2 = motion2.position.x + x2_half_scale;

	// Calculate x overlap
	return max(0.f, min(right1, right2) - max(left1, left2));
}

float WorldSystem::calculate_y_overlap(Entity entity1, Entity entity2) {
    Hitbox& hitbox1 = registry.hitboxes.get(entity1);
    Hitbox& hitbox2 = registry.hitboxes.get(entity2);
    Motion& motion1 = registry.motions.get(entity1);
    Motion& motion2 = registry.motions.get(entity2);

	float y1_half_scale = hitbox1.dimension.y / 2;
	float y2_half_scale = hitbox2.dimension.y / 2;

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

    // handle different collectibles
    Player& player = registry.players.get(entity);
	Collectible& collectible = registry.collectibles.get(entity_other);

    if (registry.collectibleTraps.has(entity_other)) {
        trapsCounter.count++;
        printf("Player collected a trap. Trap count is now %d\n", trapsCounter.count);
    }
    else if (registry.hearts.has(entity_other)) {
        unsigned int health = registry.hearts.get(entity_other).health;
        unsigned int addOn = player.health <= 80 ? health : 100 - player.health;
        player.health += addOn;
		printf("Player collected a heart\n");
	}
	else {
		printf("Unknown collectible type\n");
	}

    // destroy the collectible
    registry.remove_all_components_of(entity_other);
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
        checkAndHandlePlayerDeath(entity);
	}
	else if (registry.enemies.has(entity)) {
        printf("Enemy hit a trap\n");

        // reduce enemy health
        Enemy& enemy = registry.enemies.get(entity);
        int new_health = enemy.health - trap.damage;
        enemy.health = new_health < 0 ? 0 : new_health;
        was_damaged.push_back(entity);
        printf("Enemy health reduced from %d to %d\n", enemy.health + trap.damage, enemy.health);
		checkAndHandleEnemyDeath(entity);
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
    recoil_entities(player, enemy);

    if (!registry.cooldowns.has(enemy)) {
        Player& playerData = registry.players.get(player);
        Enemy& enemyData = registry.enemies.get(enemy);

        int newHealth = playerData.health - enemyData.damage;
        playerData.health = std::max(newHealth, 0);
        was_damaged.push_back(player);
        printf("Player health reduced by enemy from %d to %d\n", playerData.health + enemyData.damage, playerData.health);

        Cooldown& cooldown = registry.cooldowns.emplace(enemy);
        cooldown.remaining = enemyData.cooldown;

		checkAndHandlePlayerDeath(player);
    }
}

void WorldSystem::processEnemyEnemyCollision(Entity enemy1, Entity enemy2, std::vector<Entity>& was_damaged) {
    recoil_entities(enemy1, enemy2);

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
        motion.velocity = { 0, 0, motion.velocity.z }; // Stop enemy movement
        motion.angle = 1.57f; // Rotate enemy 90 degrees
        printf("Enemy %d died with health %d\n", (unsigned int)enemy, enemyData.health);

        HealthBar& hpbar = registry.healthBars.get(enemy);
        registry.remove_all_components_of(hpbar.meshEntity);
        registry.healthBars.remove(enemy);
        registry.enemies.remove(enemy);
        registry.deathTimers.emplace(enemy);
    }
}

void WorldSystem::despawn_collectibles(float elapsed_ms) {
	for (auto& collectibleEntity : registry.collectibles.entities) {
		Collectible& collectible = registry.collectibles.get(collectibleEntity);
		collectible.timer -= elapsed_ms;
		if (collectible.timer < 0) {
			registry.remove_all_components_of(collectibleEntity);
		}
	}
}

void WorldSystem::checkAndHandlePlayerDeath(Entity& entity) {
	if (registry.players.get(entity).health == 0) {
		Motion& motion = registry.motions.get(entity);
		motion.angle = 1.57f; // Rotate player 90 degrees
		printf("Player died\n");
	}
}

void WorldSystem::place_trap(Player& player, Motion& motion, bool forward) {
    // Player position
    vec2 playerPos = motion.position;
	// Do not place trap if player has no traps
    if (trapsCounter.count == 0) {
        printf("Player has no traps to place\n");
        return;
    }
	// Place trap based on player direction
    vec2 gap = { 0.0f, 0.0f };
    if (forward) {
        gap.x = (abs(motion.scale.x) / 2 + 70.f);
    }
    else {
		gap.x = -(abs(motion.scale.x) / 2 + 70.f);
    }

    // Cannot place trap beyond the map
	if (playerPos.x + gap.x < 0 || playerPos.x + gap.x > world_size_x) {
		printf("Cannot place trap beyond the map\n");
		return;
	}

    vec2 trapPos = playerPos + gap;
	createDamageTrap(renderer, trapPos);
	trapsCounter.count--;
	printf("Trap count is now %d\n", trapsCounter.count);
}

//Update player stamina on dashing, sprinting, rolling and jumping
void WorldSystem::handle_stamina(float elapsed_ms) {
    for (auto& staminaEntity : registry.staminas.entities) {
        Stamina& stamina = registry.staminas.get(staminaEntity);
        Player& player_comp = registry.players.get(staminaEntity);
        Dash& dash_comp = registry.dashers.get(staminaEntity);
        Jumper& player_jump = registry.jumpers.get(staminaEntity);
    
        if ((player_comp.isRunning || dash_comp.isDashing || player_comp.isRolling || player_jump.isJumping) && stamina.stamina > 0) {
            stamina.stamina -= elapsed_ms / 1000.0f * stamina.stamina_loss_rate;

            if (stamina.stamina < 0) {
                stamina.stamina = 0;
            }
        }
        else if (!player_comp.isRunning && !dash_comp.isDashing && !player_comp.isRolling && !player_jump.isJumping) {
            stamina.stamina += elapsed_ms / 1000.0f * stamina.stamina_recovery_rate;

            if (stamina.stamina > stamina.max_stamina) {
                stamina.stamina = stamina.max_stamina;
            }
        }
        

        if (stamina.stamina == 0) {
            player_comp.isRunning = false;
            player_comp.tryingToJump = false;
            dash_comp.isDashing = false;
            player_comp.isRolling = false;
            
        }
    }
}

void WorldSystem:: updateEntityFacing(){
    auto& motions_registry = registry.motions;
	for (int i = (int)motions_registry.components.size()-1; i>=0; --i) {
	    Motion& motion = motions_registry.components[i];
        Entity entity_i = motions_registry.entities[i];
        if(registry.boars.has(entity_i)){
            if (motion.velocity.x < 0) {
                motion.scale.x = abs(motion.scale.x); 
            } else if (motion.velocity.x > 0) {
                    motion.scale.x = -1.0f * abs(motion.scale.x);
            }

        } else{
            if (motion.velocity.x > 0) {
                motion.scale.x = abs(motion.scale.x); 
            } else if (motion.velocity.x < 0) {
                    motion.scale.x = -1.0f * abs(motion.scale.x);
            }
        }

        

    }

}

