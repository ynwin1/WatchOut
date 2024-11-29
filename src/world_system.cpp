#include "world_system.hpp"
#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "physics_system.hpp"
#include "sound_system.hpp"
#include "game_state_controller.hpp"
#include <iostream>
#include <iomanip> 
#include <sstream>
#include <fstream> 

WorldSystem::WorldSystem(std::default_random_engine& rng)
{
    this->gameStateController = GameStateController();
    this->gameStateController.init(GAME_STATE::PLAYING, this);
    this->rng = rng;
}

void WorldSystem::init(RenderSystem* renderer, GLFWwindow* window, Camera* camera, PhysicsSystem* physics, AISystem* ai, SoundSystem* sound)
{
    
    this->renderer = renderer;
    this->window = window;
    this->camera = camera;
    this->physics = physics;
    this->ai = ai;
	this->sound = sound;

    // Setting callbacks to member functions (that's why the redirect is needed)
    // Input is handled using GLFW, for more info see
    // http://www.glfw.org/docs/latest/input_guide.html
    glfwSetWindowUserPointer(window, this);
    auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
    auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
    auto mouse_button_redirect = [](GLFWwindow* wnd, int button, int action, int mods) {((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_button(button, action, mods); };
    glfwSetKeyCallback(window, key_redirect);
    glfwSetCursorPosCallback(window, cursor_pos_redirect);
    glfwSetMouseButtonCallback(window, mouse_button_redirect);
    // Window focus callback
    auto focus_redirect = [](GLFWwindow* wnd, int focused) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_window_focus(focused); };
    glfwSetWindowFocusCallback(window, focus_redirect);

    restart_game();
}

WorldSystem::~WorldSystem() {
    // Destroy all created components
    registry.clear_all_components();
}

void WorldSystem::restart_game()
{
    registry.clear_all_components();

    createMapTiles();
    createCliffs(window);
    createTrees(renderer);
    createObstacles();
    
    // Create player entity
    playerEntity = createJeff(vec2(world_size_x / 2.f, world_size_y / 2.f));
    createPlayerHealthBar(playerEntity, camera->getSize());
    createPlayerStaminaBar(playerEntity, camera->getSize());

    gameStateController.setGameState(GAME_STATE::PLAYING);
    show_mesh = false;
    resetSpawnSystem();
    initText();
    soundSetUp();

    gameStateController.inventory.itemCounts[INVENTORY_ITEM::BOW] = 1;

    // Set spawn delays to 1 second, so the first of each type will spawn right away
    for (auto& name : entity_types) {
        next_spawns[name] = 1000;
    }
    loadAndSaveHighScore(false);
}

void WorldSystem::updateGameTimer(float elapsed_ms) {
    GameTimer& gameTimer = registry.gameTimer;

    gameTimer.update(elapsed_ms);
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << gameTimer.hours << ":"
       << std::setw(2) << std::setfill('0') << gameTimer.minutes << ":"
       << std::setw(2) << std::setfill('0') << gameTimer.seconds;

    Text& text = registry.texts.get(gameTimer.textEntity);
    text.value = ss.str();
}

void WorldSystem::initText() {
    createPauseHelpText(camera->getSize());
    registry.fpsTracker.textEntity = createFPSText(camera->getSize());
    registry.gameTimer.reset();
    registry.gameTimer.textEntity = createGameTimerText(camera->getSize());
    
    gameStateController.inventory.reset();
    std::unordered_map<INVENTORY_ITEM, Entity>& invItemEntities = gameStateController.inventory.itemEntities;
    vec2 inventoryItemAnchorPos = {420.0f, camera->getSize().y - 30.0f};
    invItemEntities[INVENTORY_ITEM::TRAP] = createItemCountText(inventoryItemAnchorPos, TEXTURE_ASSET_ID::TRAPCOLLECTABLE, { TRAP_COLLECTABLE_BB_WIDTH * 1.25, TRAP_COLLECTABLE_BB_HEIGHT * 1.25 }, 1);
    invItemEntities[INVENTORY_ITEM::BOW] = createItemCountText(inventoryItemAnchorPos + vec2(80.0f, 0.0f), TEXTURE_ASSET_ID::BOW, { BOW_BB_WIDTH * 0.7, BOW_BB_HEIGHT * 0.7}, 2);
}

void WorldSystem::trackFPS(float elapsed_ms) {
    FPSTracker& fpsTracker = registry.fpsTracker; 
    fpsTracker.update(elapsed_ms);

    if(fpsTracker.elapsedTime == 0) {
        Text& text = registry.texts.get(fpsTracker.textEntity);
        text.value = std::to_string(fpsTracker.fps) + " fps";
    }
}

void WorldSystem::updateInventoryItemText() {
    Inventory& inventory = gameStateController.inventory;
    for (auto& item : inventory.itemEntities) {
        if(registry.texts.has(item.second)) {
            Text& text = registry.texts.get(item.second);
            std::stringstream ss;
            ss << std::setw(2) << std::setfill('0') << inventory.itemCounts[item.first];
            text.value = "*" + ss.str();

            if(inventory.itemCounts[item.first] == 0) {
                registry.colours.get(item.second) = {0.8f, 0.8f, 0.0f, 1.0f};
            } else {
                registry.colours.get(item.second) = {1.0f, 1.0f, 1.0f, 1.0f};
            }
        }
    }
}

void WorldSystem::updateCollectedTimer(float elapsed_ms) {
    for (Entity entity : registry.collected.entities) {
        Collected& collected = registry.collected.get(entity);
        collected.duration -= elapsed_ms;
        if (collected.duration < 0) {
            registry.remove_all_components_of(entity);
        }
    }
}

void WorldSystem::updateEquippedPosition() {
	Entity& playerE = registry.players.entities[0];
	Motion& playerM = registry.motions.get(playerE);

    if(registry.equipped.has(gameStateController.inventory.equippedEntity)) {
        Motion& equippedM = registry.motions.get(gameStateController.inventory.equippedEntity);
        equippedM.position = playerM.position;
        
        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);
        vec3 mouseWorldPos = renderer->mouseToWorld({mousePosX, mousePosY});

        const float fixedDistance = abs(playerM.scale.x) / 2;

        vec3 direction = mouseWorldPos - playerM.position;
        vec3 normalizedDirection = normalize(direction);

        equippedM.position = playerM.position + normalizedDirection * fixedDistance;

        if(gameStateController.inventory.equipped == INVENTORY_ITEM::BOW) {
            float angle = atan2(direction.y, direction.x);
            equippedM.angle = angle;
        }
    }
}

bool WorldSystem::step(float elapsed_ms)
{
    adjustSpawnSystem(elapsed_ms);
    spawn(elapsed_ms);
    update_cooldown(elapsed_ms);
    handle_deaths(elapsed_ms);
    despawn_collectibles(elapsed_ms);
	destroyDamagings();
    handle_stamina(elapsed_ms);
    trackFPS(elapsed_ms);
    updateGameTimer(elapsed_ms);
    updateInventoryItemText();
    toggleMesh();
    accelerateFireballs(elapsed_ms);
    despawnTraps(elapsed_ms);
    updateCollectedTimer(elapsed_ms);
    resetTrappedEntities();
    updateHomingProjectiles(elapsed_ms);
    updateEquippedPosition();

    if (camera->isToggled()) {
        Motion& playerMotion = registry.motions.get(playerEntity);
        camera->followPosition(vec2(playerMotion.position.x, playerMotion.position.y * yConversionFactor));
    }


    Player& player = registry.players.get(playerEntity);
    if(player.health == 0) {
        loadAndSaveHighScore(true);
        gameStateController.setGameState(GAME_STATE::GAMEOVER);
    }

    return !is_over();
}

void WorldSystem::loadAndSaveHighScore(bool save) {
    std::string filename = "highscore.txt";
    GameTimer& gameTimer = registry.gameTimer;
    GameScore& gameScore = registry.gameScore;
    if (save) {
        if (gameTimer.hours > gameScore.highScoreHours || 
            (gameTimer.hours == gameScore.highScoreHours && gameTimer.minutes > gameScore.highScoreMinutes) ||
            (gameTimer.hours == gameScore.highScoreHours && gameTimer.minutes == gameScore.highScoreMinutes && gameTimer.seconds > gameScore.highScoreSeconds)) {
            
            // Update high score
            gameScore.highScoreHours = gameTimer.hours;
            gameScore.highScoreMinutes = gameTimer.minutes;
            gameScore.highScoreSeconds = gameTimer.seconds;

            std::ofstream file(filename);
            if (file.is_open()) {
                file << gameScore.highScoreHours << " " <<gameScore.highScoreMinutes << " " << gameScore.highScoreSeconds;
                file.close();
            }
        }
    } else {
        std::ifstream file(filename);
        if (file.is_open()) {
            file >> gameScore.highScoreHours >> gameScore.highScoreMinutes >> gameScore.highScoreSeconds;
            file.close();
        } else {
            //if file doesnt exist (shouldn't be an issue)
            gameScore.highScoreHours = 0;
            gameScore.highScoreMinutes = 0;
            gameScore.highScoreSeconds = 0;
        }
    }
}

void WorldSystem::handle_collisions()
{
    std::vector<Entity> was_damaged;
    // Loop over all collisions detected by the physics system
    for (uint i = 0; i < physics->collisions.size(); i++) {
        // The entity and its collider
        Entity entity = physics->collisions[i].first;
        Entity entity_other = physics->collisions[i].second;

        if (registry.traps.has(entity_other) && (registry.players.has(entity) || registry.enemies.has(entity))) {
            entity_trap_collision(entity, entity_other, was_damaged);
        }

        float COOLDOWN_TIME = 1000;
        std::pair<int, int> pair = { entity, entity_other };
        if (collisionCooldowns.find(pair) != collisionCooldowns.end()) {
            continue;
        }
        else {
            collisionCooldowns[pair] = COOLDOWN_TIME;
        }

        // If the entity is a player
        if (registry.players.has(entity)) {
            // If the entity is colliding with a collectible
            if (registry.collectibles.has(entity_other)) {
				entity_collectible_collision(entity, entity_other);
            }
            else if (registry.enemies.has(entity_other)) {
				// Collision between player and enemy
                processPlayerEnemyCollision(entity, entity_other, was_damaged);
            }
            else if (registry.damagings.has(entity_other)) {
                entity_damaging_collision(entity, entity_other, was_damaged);
            }
        }
        else if (registry.enemies.has(entity)) {
            if (registry.enemies.has(entity_other)) {
				// Collision between two enemies
				handleEnemyCollision(entity, entity_other, was_damaged);
            }
            else if (registry.damagings.has(entity_other)) {
                entity_damaging_collision(entity, entity_other, was_damaged);
            }
            else if (registry.obstacles.has(entity_other)) {
                entity_obstacle_collision(entity, entity_other, was_damaged);
            }
        }
        else if (registry.damagings.has(entity)) {
			Damaging& damaging = registry.damagings.get(entity);
            if (damaging.type == "fireball" && registry.obstacles.has(entity_other)) {
				// Collision between damaging and obstacle
                damaging_obstacle_collision(entity);
            }
        }
    }

    // Handle deaths after all collisions are handled
    for (Entity enemy : registry.enemies.entities) {
        checkAndHandleEnemyDeath(enemy);
    }
    for (Entity player : registry.players.entities) {
        checkAndHandlePlayerDeath(player);
    }

    // Clear all collisions
    renderer->turn_damaged_red(was_damaged);
    physics->collisions.clear();
}

void WorldSystem::resetTrappedEntities() {
    for (Entity entity : registry.trappables.entities) {
        if (registry.motions.has(entity)) {
            Trappable& trappable = registry.trappables.get(entity);
            trappable.isTrapped = false;
            registry.motions.get(entity).speed = trappable.originalSpeed;
        }
    }
}

// Should the game be over ?
bool WorldSystem::is_over() const {
    return bool(glfwWindowShouldClose(window));
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {

}

void WorldSystem::shootHomingArrow(Entity targetEntity, float angle) {
    Motion& targetM = registry.motions.get(targetEntity);
    Motion& motion = registry.motions.get(registry.players.entities.at(0));

    vec2 direction = normalize(vec2(targetM.position) - vec2(motion.position));
    vec3 pos = motion.position;

    float x_offset = FIREBALL_HITBOX_WIDTH + motion.hitbox.x / 2;
    float y_offset = FIREBALL_HITBOX_WIDTH + motion.hitbox.y / 2;
	// travelling more horizontally so no y offset
    if (abs(direction.x) > abs(direction.y)) {
        y_offset = 0;
    }
    else if (abs(direction.x) < abs(direction.y)) {
        x_offset = 0;
    }
    // offset must be on the left if travelling left
    if (direction.x < 0) {
        x_offset = -x_offset;
    }
    if (direction.y < 0) {
        y_offset = -y_offset;
    }
    pos += vec3(x_offset, y_offset, 0);

    Entity arrowE = createArrow(pos, vec3(0), PLAYER_ARROW_DAMAGE);
    registry.motions.get(arrowE).angle = angle;
    registry.homingProjectiles.emplace(arrowE, targetEntity).speed = HOMING_ARROW_SPEED;
    registry.playerDamagings.emplace(arrowE);
}

void WorldSystem::shotArchingArrow(vec3 targetPos) {
    // Always shoot arrow at 45 degree angle (makes calculations simpler)
    const float ARROW_ANGLE = M_PI / 4;
    const float MAX_ARROW_VELOCITY = 10;

    Motion& motion = registry.motions.get(playerEntity);

    // Get start position of the arrow
    vec2 horizontal_direction = normalize(vec2(targetPos) - vec2(motion.position));
    const float maxArrowDimension = max(ARROW_BB_HEIGHT, ARROW_BB_WIDTH);
    vec3 pos = motion.position;
    if (abs(horizontal_direction.x) > abs(horizontal_direction.y)) {
        if (horizontal_direction.x > 0) {
            pos.x += motion.hitbox.x / 2 + maxArrowDimension;
        }
        else if (horizontal_direction.x < 0) {
            pos.x -= motion.hitbox.x / 2 + maxArrowDimension;
        }
    }
    else {
        if (horizontal_direction.y > 0) {
            pos.y += motion.hitbox.y / 2 + maxArrowDimension;
        }
        else if (horizontal_direction.x < 0) {
            pos.y -= motion.hitbox.y / 2 + maxArrowDimension;
        }
    }
    pos.z += motion.hitbox.z / 2 + maxArrowDimension;
    horizontal_direction = normalize(vec2(targetPos) - vec2(pos));

    // Get distances from start to target
    float horizontal_distance = distance(vec2(pos), vec2(targetPos));
    float vertical_distance = targetPos.z - pos.z;

    // Prevent trying to shoot above what is possible
    if (vertical_distance >= horizontal_distance)
        return;

    float velocity = horizontal_distance * sqrt(-GRAVITATIONAL_CONSTANT / (vertical_distance - horizontal_distance));

    // Prevent shooting at crazy speeds
    if (velocity > MAX_ARROW_VELOCITY)
        return;

    // Determine velocities for each dimension
    vec2 horizontal_velocity = velocity * cos(ARROW_ANGLE) * horizontal_direction;
    float vertical_velocity = velocity * sin(ARROW_ANGLE);
    Entity arrowE = createArrow(pos, vec3(horizontal_velocity, vertical_velocity), PLAYER_ARROW_DAMAGE);
    registry.playerDamagings.emplace(arrowE);
}

void WorldSystem::shootArrow(vec3 mouseWorldPos) {
    vec3 playerPos = registry.motions.get(playerEntity).position;

    float birdClicked = false;

    for(Entity birdE : registry.birds.entities) {
        if(registry.deathTimers.has(birdE)) {
            continue;
        }

        Motion& birdM = registry.motions.get(birdE);

        vec2 visualPos = worldToVisual(birdM.position);
        vec2 worldPos = {visualPos.x, visualToWorldY(visualPos.y)};
        vec2 birdBBTopLeft = {worldPos.x - BIRD_BB_WIDTH / 2, worldPos.y - BIRD_BB_HEIGHT / 2};
        vec2 birdBBBottomRight = {worldPos.x + BIRD_BB_WIDTH / 2, worldPos.y + BIRD_BB_HEIGHT / 2};
        // apply AABB
        birdClicked = birdBBTopLeft.x < mouseWorldPos.x &&
                    birdBBTopLeft.y < mouseWorldPos.y &&
                    birdBBBottomRight.x > mouseWorldPos.x &&
                    birdBBBottomRight.y > mouseWorldPos.y;
        if(birdClicked) {
            vec2 direction = mouseWorldPos - playerPos;
            float angle = atan2(direction.y, direction.x);
            shootHomingArrow(birdE, angle);
            break;
        }
    }

    if(!birdClicked) {
        shotArchingArrow(mouseWorldPos);
    }

    sound->playSoundEffect(Sound::ARROW, 0);
}

void WorldSystem::shootProjectile(vec3 mouseWorldPos) {
    switch(gameStateController.inventory.equipped) {
        case INVENTORY_ITEM::BOW:
            shootArrow(mouseWorldPos);
            break;
    }
}

void WorldSystem::on_mouse_button(int button, int action, int mod) {
    if (action == GLFW_PRESS) {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos); // get the current cursor position
            vec3 mouseWorldPos = renderer->mouseToWorld({xpos, ypos});

            shootProjectile(mouseWorldPos);
        break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            gameStateController.inventory.unequip();
        break;
        }
    }
}

void WorldSystem::on_key(int key, int, int action, int mod)
{
    switch (gameStateController.getGameState()) {
    case GAME_STATE::PLAYING:
        playingControls(key, action, mod);
        break;
    case GAME_STATE::PAUSED:
		sound->isBirdFlockSoundPlaying = false;
		sound->isMovingSoundPlaying = false;
        sound->pauseAllSoundEffects();
        pauseControls(key, action, mod);
        break;
    case GAME_STATE::GAMEOVER:
        gameOverControls(key, action, mod);
        break;
    case GAME_STATE::HELP:
        sound->isMovingSoundPlaying = false;
        sound->isBirdFlockSoundPlaying = false;
        sound->pauseAllSoundEffects();
        helpControls(key, action, mod);
        break;
    }
    allStateControls(key, action, mod);
    movementControls(key, action, mod);
}

void WorldSystem::helpControls(int key, int action, int mod)
{
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_Q:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_ENTER:
            restart_game();
        case GLFW_KEY_H:
            gameStateController.setGameState(GAME_STATE::PLAYING);
            sound->resumeAllSoundEffects();
            break;
        case GLFW_KEY_P:
        case GLFW_KEY_ESCAPE:
            gameStateController.setGameState(GAME_STATE::PAUSED);
            break;
        }
    }
}

void WorldSystem::pauseControls(int key, int action, int mod)
{
    // Close the game
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_Q:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_H:
            gameStateController.setGameState(GAME_STATE::HELP);
            break;
        case GLFW_KEY_ENTER:
            restart_game();
        case GLFW_KEY_P:
        case GLFW_KEY_ESCAPE:
            gameStateController.setGameState(GAME_STATE::PLAYING);
			sound->resumeAllSoundEffects();
            break;
        }
    }
}

void WorldSystem::playingControls(int key, int action, int mod)
{
    Player& player_comp = registry.players.get(playerEntity);
    Motion& player_motion = registry.motions.get(playerEntity);
  
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_W:
            place_trap(player_comp, player_motion, true);
            break;
        case GLFW_KEY_Q:
            place_trap(player_comp, player_motion, false);
            break;
        case GLFW_KEY_1:
            gameStateController.inventory.equipItem(INVENTORY_ITEM::BOW);
            break;
        case GLFW_KEY_H:
            gameStateController.setGameState(GAME_STATE::HELP);
            break;
        case GLFW_KEY_P:
        case GLFW_KEY_ESCAPE:
            gameStateController.setGameState(GAME_STATE::PAUSED);
            break;
        }
    }
}

void WorldSystem::gameOverControls(int key, int action, int mod)
{
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ENTER:
            restart_game();
            break;
        case GLFW_KEY_Q:
            glfwSetWindowShouldClose(window, true);
        }
    }
}


void WorldSystem::allStateControls(int key, int action, int mod)
{
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_C:
            // toggle camera on/off for debugging/testing
            camera->toggle();
            break;
        case GLFW_KEY_F:
            // toggle fps
            registry.fpsTracker.toggled = !registry.fpsTracker.toggled;
            break;
		case GLFW_KEY_M:
            // toggle sound
			sound->mute = !sound->mute;
			if (sound->mute) {
				sound->muteAllSounds();
			}
			else {
				sound->unmuteAllSounds();
			}
            break;
        case GLFW_KEY_V:
            isWindowed = !isWindowed;
            GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
            if (isWindowed) {
                glfwSetWindowMonitor(window, nullptr, 50, 50, mode->width, mode->height, 0);
            }
            else {
                glfwSetWindowMonitor(window, primaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            }
            glfwSwapInterval(1); // vsync
            break;
        }
    }
}

void WorldSystem::movementControls(int key, int action, int mod)
{
    Player& player_comp = registry.players.get(playerEntity);
    Motion& player_motion = registry.motions.get(playerEntity);
    Dash& player_dash = registry.dashers.get(playerEntity);
    Stamina& player_stamina = registry.staminas.get(playerEntity);
    Trappable& player_trappable = registry.trappables.get(playerEntity);

    if (action != GLFW_PRESS && action != GLFW_RELEASE) {
        return;
    }

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
        }
        else {
            player_comp.isRunning = false;
        }
        break;
    case GLFW_KEY_R:
        // Roll
        if (player_stamina.stamina > 0) {
            player_comp.isRolling = pressed;
        }
        else {
            player_comp.isRolling = false;
        }
        break;
    case GLFW_KEY_D:
        if (pressed) {
            if (player_stamina.stamina > DASH_STAMINA) {
                const float dashDistance = 300;
                // Start dashing if player is moving
                player_dash.isDashing = true;
                player_dash.dashStartPosition = vec2(player_motion.position);
                player_dash.dashTargetPosition = player_dash.dashStartPosition + player_motion.facing * dashDistance;
                player_dash.dashTimer = 0.0f; // Reset timer
                player_stamina.stamina -= DASH_STAMINA;

                // play dash sound
                sound->playSoundEffect(Sound::DASHING, 0);
            }
        }
        break;
    case GLFW_KEY_SPACE:
        // Jump
        if (pressed && !player_trappable.isTrapped) {
            if (player_stamina.stamina >= JUMP_STAMINA && !player_comp.tryingToJump) {
                player_comp.tryingToJump = true;
                if (registry.jumpers.has(playerEntity)) {
                    Jumper& jumper = registry.jumpers.get(playerEntity);
                    if (!jumper.isJumping) {
                        jumper.isJumping = true;
                        player_comp.tryingToJump = true;
                        player_motion.velocity.z = jumper.speed;
                        player_stamina.stamina -= JUMP_STAMINA;
                        if (player_stamina.stamina < 0) {
                            player_stamina.stamina = 0;
                        }
                        // play jump sound
                        sound->playSoundEffect(Sound::JUMPING, 0);
                    }
                }
            }
        }
        else {
            player_comp.tryingToJump = false;
        }
        break;
    default:
        break;
    }
    update_player_facing(player_comp, player_motion);
}

void WorldSystem::update_player_facing(Player& player, Motion& motion) 
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
        motion.facing = normalize(player_facing);
    }
}

void WorldSystem::despawnTraps(float elapsed_ms) {
    for (Entity& trapE : registry.traps.entities) {
        Trap& trap = registry.traps.get(trapE);
        trap.duration -= elapsed_ms;
        if (trap.duration <= 0) {
            registry.remove_all_components_of(trapE);
        }
    }
}

void WorldSystem::update_cooldown(float elapsed_ms) {
    // Tick type-specific cooldowns
    for (auto& cooldownEntity : registry.cooldowns.entities) {
        Cooldown& cooldown = registry.cooldowns.get(cooldownEntity);
        cooldown.remaining -= elapsed_ms;

        if (cooldown.remaining <= 0) {
            // remove lightning
            if (registry.damagings.has(cooldownEntity) && registry.damagings.get(cooldownEntity).type == "lightning") {
                registry.remove_all_components_of(cooldownEntity);
            }
            // remove target area
            else if (registry.targetAreas.has(cooldownEntity)) {
                registry.remove_all_components_of(cooldownEntity);
            }
            else {
                registry.cooldowns.remove(cooldownEntity);
            }
        }
    }

    // Tick general collision cooldowns
    auto it = collisionCooldowns.begin();
    while (it != collisionCooldowns.end()) {
        it->second -= elapsed_ms;
        if (it->second <= 0) {
            it = collisionCooldowns.erase(it);
        }
        else {
            it++;
        }
    }
}

void WorldSystem::handle_deaths(float elapsed_ms) {
    for (auto& deathEntity : registry.deathTimers.entities) {
        DeathTimer& deathTimer = registry.deathTimers.get(deathEntity);
        deathTimer.timer -= elapsed_ms;
        if (deathTimer.timer < 0) {
            // Remove
            if (registry.motions.has(deathEntity)) {
                Motion& motion = registry.motions.get(deathEntity);
                createHeart({ motion.position.x, motion.position.y });
            }
            registry.remove_all_components_of(deathEntity);
        }
    }
}

void WorldSystem::spawn(float elapsed_ms)
{
    for (std::string& entity_type : entity_types) {
        next_spawns.at(entity_type) -= elapsed_ms;
        int maxEntitySize = max_entities.at(entity_type);
		int currentEntitySize = registry.spawnable_lists.at(entity_type)->size();
        if (registry.enemies.size() < MAX_TOTAL_ENEMIES && 
            next_spawns.at(entity_type) < 0 && 
            currentEntitySize < maxEntitySize) 
        {
            printf("Spawning: %s\n", entity_type.c_str());
            vec2 spawnLocation = get_spawn_location(entity_type);
            spawn_func f = spawn_functions.at(entity_type);
            (*f)(spawnLocation);
            next_spawns[entity_type] = spawn_delays.at(entity_type);
        }
    }
}

vec2 WorldSystem::get_spawn_location(const std::string& entity_type)
{
    vec2 spawn_location{};

    // spawn collectibles
	if (entity_type == "heart" || entity_type == "collectible_trap") {
		// spawn at random location on the map
        float posX = uniform_dist(rng) * (rightBound - leftBound) + leftBound;
        float posY = uniform_dist(rng) * (bottomBound - topBound) + topBound;
        spawn_location = { posX, posY };
    }
    else 
	// spawn enemies
    {
        // Do not spawn within camera's view (with some margins)
        float exclusionTop = (camera->getPosition().y - camera->getSize().y / 2) / yConversionFactor - 100;
        float exclusionBottom = (camera->getPosition().y + camera->getSize().y / 2) / yConversionFactor + 400;
        float exclusionLeft = camera->getPosition().x - camera->getSize().x / 2 - 100;
        float exclusionRight = camera->getPosition().x + camera->getSize().x / 2 + 100;

        float posX = uniform_dist(rng) * (rightBound - leftBound) + leftBound;
        float posY = uniform_dist(rng) * (bottomBound - topBound) + topBound;
        // Spawning in exclusion zone
        if (posX < exclusionRight && posX > exclusionLeft &&
            posY < exclusionBottom && posY > exclusionTop)
        {
            if (exclusionTop > topBound) {
                posY = exclusionTop;
            }
            else {
                posY = exclusionBottom;
            }
        }
        spawn_location = { posX, posY };
    }
    return spawn_location;
}

// Collision functions
void WorldSystem::entity_collectible_collision(Entity entity, Entity entity_other) {
    // ONLY PLAYER CAN COLLECT COLLECTIBLES

    // handle different collectibles
    Player& player = registry.players.get(entity);
    Motion& playerM = registry.motions.get(entity);
    Motion& collectibleM = registry.motions.get(entity_other);
	Collectible& collectible = registry.collectibles.get(entity_other);

    if (registry.collectibleTraps.has(entity_other)) {
        gameStateController.inventory.itemCounts[INVENTORY_ITEM::TRAP]++;
        createCollected(playerM, collectibleM.scale, TEXTURE_ASSET_ID::TRAPCOLLECTABLE);
        printf("Player collected a trap. Trap count is now %d\n", gameStateController.inventory.itemCounts[INVENTORY_ITEM::TRAP]);
    }
    else if (registry.hearts.has(entity_other)) {
        unsigned int health = registry.hearts.get(entity_other).health;
        unsigned int addOn = player.health <= 80 ? health : 100 - player.health;
        player.health += addOn;
        createCollected(playerM, collectibleM.scale, TEXTURE_ASSET_ID::HEART);
		printf("Player collected a heart\n");
	}
	else {
		printf("Unknown collectible type\n");
	}

	sound->playSoundEffect(Sound::COLLECT, 0);
    // destroy the collectible
    registry.remove_all_components_of(entity_other);
}

void WorldSystem::entity_trap_collision(Entity entity, Entity entity_other, std::vector<Entity>& was_damaged) {
    Trap& trap = registry.traps.get(entity_other);

    if (registry.trappables.has(entity)) {
        Trappable& trappable = registry.trappables.get(entity);
        Motion& motion = registry.motions.get(entity);

        // apply slow effect
        motion.speed *= trap.slowFactor;
        trappable.isTrapped = true;

        // if boar is charging, stop mid-charge 
        if(registry.boars.has(entity)) {
            registry.boars.get(entity).charging = false;
        }
	}
}

void WorldSystem::entity_damaging_collision(Entity entity, Entity entity_other, std::vector<Entity>& was_damaged)
{
    Damaging& damaging = registry.damagings.get(entity_other);

    if (registry.players.has(entity)) {
        // prevent player taking damage from own damaging object
        if(registry.playerDamagings.has(entity_other)) {
            return;
        }
        // reduce player health
        Player& player = registry.players.get(entity);
        int new_health = player.health - damaging.damage;
        player.health = new_health < 0 ? 0 : new_health;
        was_damaged.push_back(entity);
        printf("Player health reduced from %d to %d\n", player.health + damaging.damage, player.health);
    }
    else if (registry.enemies.has(entity)) {
        // reduce enemy health
        Enemy& enemy = registry.enemies.get(entity);
        enemy.health -= damaging.damage;
        was_damaged.push_back(entity);
        printf("Enemy health reduced from %d to %d by damaging object\n", enemy.health + damaging.damage, enemy.health);
    }
    else {
        printf("Entity is not a player or enemy\n");
        return;
    }

    registry.remove_all_components_of(entity_other);
}

void WorldSystem::damaging_obstacle_collision(Entity damaging) {
    // Currently, there is only fireball
	registry.remove_all_components_of(damaging);
}
  
void WorldSystem::entity_obstacle_collision(Entity entity, Entity obstacle, std::vector<Entity>& was_damaged)
{
    if (registry.boars.has(entity)) {
        Boar& boar = registry.boars.get(entity);
        if (boar.charging) {
           Enemy& enemy = registry.enemies.get(entity);
            // Boar hurts itself
           enemy.health -= enemy.damage / 2; // half damage of what it does to other entities
           ai->boarReset(entity);
           boar.cooldownTimer = 1000; // stunned for 1 second
           
           was_damaged.push_back(entity);
        }
    }
}

void WorldSystem::processPlayerEnemyCollision(Entity player, Entity enemy, std::vector<Entity>& was_damaged) {
    // Archers do not do melee damage
    if (registry.archers.has(enemy)) {
        return;
    }

    if (!registry.cooldowns.has(enemy)) {
        Player& playerData = registry.players.get(player);
        Enemy& enemyData = registry.enemies.get(enemy);

        int newHealth = playerData.health - enemyData.damage;
        playerData.health = std::max(newHealth, 0);
        was_damaged.push_back(player);
        printf("Player health reduced by enemy from %d to %d\n", playerData.health + enemyData.damage, playerData.health);

        // Check if enemy can have an attack cooldown
        if (enemyData.cooldown > 0) {
            Cooldown& cooldown = registry.cooldowns.emplace(enemy);
            cooldown.remaining = enemyData.cooldown;
        }

        knock(player, enemy);
    }
}

void WorldSystem::handleEnemyCollision(Entity attacker, Entity target, std::vector<Entity>& was_damaged) {
    if (!registry.cooldowns.has(attacker)) {
        Enemy& attackerData = registry.enemies.get(attacker);
        Enemy& targetData = registry.enemies.get(target);

        bool apply = false;

        if (registry.boars.has(attacker)) {
            Boar& boar = registry.boars.get(attacker);
            if (boar.charging) {
                // damage should only apply when boar is charging 
                // (boars can be colliding with others while walking)
                apply = true;
            }
        }
        else if (registry.trolls.has(attacker) && !registry.trolls.has(target)) {
            apply = true;
        }
        else if (registry.birds.has(attacker) && !registry.birds.has(target)) {
            apply = true;
        }

        if (!apply) {
            return;
        }

        targetData.health -= attackerData.damage;
        was_damaged.push_back(target);
        printf("Enemy %d's health reduced from %d to %d by enemy\n", (unsigned int)target, targetData.health + attackerData.damage, targetData.health);

        if (attackerData.cooldown > 0) {
            Cooldown& cooldown = registry.cooldowns.emplace(attacker);
            cooldown.remaining = attackerData.cooldown;
        }

        knock(target, attacker);
    }
}

void WorldSystem::checkAndHandleEnemyDeath(Entity enemy) {
    Enemy& enemyData = registry.enemies.get(enemy);
    if (enemyData.health <= 0 && !registry.deathTimers.has(enemy)) {
        Motion& motion = registry.motions.get(enemy);
        // Do not rotate wizard
        if (!registry.wizards.has(enemy)) {
            motion.angle = M_PI / 2; // Rotate enemy 90 degrees
            motion.hitbox = { motion.hitbox.z, motion.hitbox.y, motion.hitbox.x }; // Change hitbox to be on its side
        }
        printf("Enemy %d died with health %d\n", (unsigned int)enemy, enemyData.health);

        if (registry.animationControllers.has(enemy)) {
            AnimationController& animationController = registry.animationControllers.get(enemy);
            animationController.changeState(enemy, AnimationState::Dead);
        }

        HealthBar& hpbar = registry.healthBars.get(enemy);
        registry.remove_all_components_of(hpbar.meshEntity);
        registry.remove_all_components_of(hpbar.frameEntity);
        registry.healthBars.remove(enemy);
        registry.enemies.remove(enemy);
        registry.deathTimers.emplace(enemy);
    }
}

void WorldSystem::knock(Entity knocked, Entity knocker)
{
    // Skip if entities are not suitable
    if (!registry.knockables.has(knocked) || !registry.knockers.has(knocker)) {
        return;
    }

    const float KNOCK_ANGLE = M_PI / 4;  // 45 degrees
    float strength = registry.knockers.get(knocker).strength;

    Motion& knockedMotion = registry.motions.get(knocked);
    Motion& knockerMotion = registry.motions.get(knocker);
    vec2 horizontal_direction = normalize(vec2(knockedMotion.position) - vec2(knockerMotion.position));
    vec3 d = normalize(vec3(horizontal_direction * cos(KNOCK_ANGLE), sin(KNOCK_ANGLE)));
    knockedMotion.velocity = d * strength;
    knockedMotion.position.z += 1; // move a little over ground to prevent being considered "on the ground" for this frame
}

void WorldSystem::despawn_collectibles(float elapsed_ms) {
	for (auto& collectibleEntity : registry.collectibles.entities) {
		Collectible& collectible = registry.collectibles.get(collectibleEntity);
		collectible.timer -= elapsed_ms;

        if (collectible.timer < 0) {
			registry.remove_all_components_of(collectibleEntity);
		} else if(collectible.timer <= collectible.duration / 2) {
            AnimationController& animatedCollectible = registry.animationControllers.get(collectibleEntity);
            animatedCollectible.changeState(collectibleEntity, AnimationState::Fading);
        }
	}
}

void WorldSystem::destroyDamagings() {
    for (auto& damagingEntity : registry.damagings.entities) {
        Damaging& damaging = registry.damagings.get(damagingEntity);
        Motion& motion = registry.motions.get(damagingEntity);

        // half scale
		float halfScaleX = abs(motion.scale.x) / 2;
		float halfScaleY = abs(motion.scale.y) / 2;

		bool collidesWithLeft = motion.position.x - halfScaleX <= leftBound;
		bool collidesWithRight = motion.position.x + halfScaleX >= rightBound;
		bool collidesWithTop = motion.position.y - halfScaleY <= topBound;
		bool collidesWithBottom = motion.position.y + halfScaleY >= bottomBound;

		// Destroy if it collides with the map bounds (fireball)
        if (collidesWithLeft || collidesWithRight || collidesWithTop || collidesWithBottom) {
            registry.remove_all_components_of(damagingEntity);
        }
    }
}

void WorldSystem::checkAndHandlePlayerDeath(Entity& entity) {
	if (registry.players.get(entity).health == 0) {
		Motion& motion = registry.motions.get(entity);
		motion.angle = M_PI / 2; // Rotate player 90 degrees
        motion.hitbox = { motion.hitbox.z, motion.hitbox.y, motion.hitbox.x }; // Change hitbox to be on its side

        sound->stopAllSounds();
		sound->playMusic(Music::PLAYER_DEATH, -1);
	}
}

void WorldSystem::place_trap(Player& player, Motion& motion, bool forward) {
    // Player position
    vec2 playerPos = motion.position;
    int& trapsCount = gameStateController.inventory.itemCounts[INVENTORY_ITEM::TRAP];
	// Do not place trap if player has no traps
    if (trapsCount == 0) {
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
	createDamageTrap(trapPos);
	trapsCount--;
	printf("Trap count is now %d\n", trapsCount);
}

//Update player stamina on dashing, sprinting, rolling and jumping
void WorldSystem::handle_stamina(float elapsed_ms) {
    for (auto& staminaEntity : registry.staminas.entities) {
        Stamina& stamina = registry.staminas.get(staminaEntity);
        Player& player_comp = registry.players.get(staminaEntity);
        Dash& dash_comp = registry.dashers.get(staminaEntity);
        Jumper& player_jump = registry.jumpers.get(staminaEntity);
    
        if (player_comp.isRunning && stamina.stamina > 0) {
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

void WorldSystem::toggleMesh() {
    // remove current meshes (every mesh has a render request)
    // replace with appropriate textures
    for (auto& meshEntity : registry.meshPtrs.entities) {
        registry.renderRequests.remove(meshEntity);
        if (show_mesh) {
            registry.renderRequests.insert(
                meshEntity, {
                    TEXTURE_ASSET_ID::TREE,
                    EFFECT_ASSET_ID::TREE,
                    GEOMETRY_BUFFER_ID::TREE });
        }
        else {
            registry.renderRequests.insert(
                meshEntity, {
                    TEXTURE_ASSET_ID::TREE,
                    EFFECT_ASSET_ID::TEXTURED,
                    GEOMETRY_BUFFER_ID::SPRITE });
        }
    }
}

void WorldSystem::adjustSpawnSystem(float elapsed_ms) {
	GameTimer& gameTimer = registry.gameTimer;
	if (gameTimer.elapsed > DIFFICULTY_INTERVAL) {
		// Increase difficulty
		for (auto& spawnDelay : spawn_delays) {
			// increse spawn delay for collectibles
			if (spawnDelay.first == "heart" || spawnDelay.first == "collectible_trap") {
				spawnDelay.second *= 1.1f;
            }
            else {
				// Decrease spawn delay for enemies
                spawnDelay.second *= 0.9f;
            }
			
		}
		for (auto& maxEntity : max_entities) {
			// Do not increase max entities for collectibles
			if (maxEntity.first == "heart" || maxEntity.first == "collectible_trap") {
				continue;
			}
			maxEntity.second++;
		}
		gameTimer.elapsed = 0;
        sound->playSoundEffect(Sound::LEVELUP, 0);
	}
}

void WorldSystem::resetSpawnSystem() {
	// Reset spawn delays
    spawn_delays = initial_spawn_delays;

	// Reset max entities
    max_entities = initial_max_entities;
}

// Pause the game when the window loses focus
void WorldSystem::on_window_focus(int focused) {
    if (focused == GLFW_FALSE) {
        if (gameStateController.getGameState() == GAME_STATE::PLAYING) {
            gameStateController.setGameState(GAME_STATE::PAUSED);
        }
    }
}

void WorldSystem::accelerateFireballs(float elapsed_ms) {
    for (auto entity : registry.damagings.entities) {
        Damaging& dmgEntity = registry.damagings.get(entity);
        if (dmgEntity.type == "fireball") {
            Motion& fireballMotion = registry.motions.get(entity);

            // calculate direction from angle
            vec2 direction = vec2(cos(fireballMotion.angle), sin(fireballMotion.angle));
            direction = normalize(direction);

            // accelerate in the calculated direction
            fireballMotion.velocity.x += (direction.x) * FIREBALL_ACCELERATION * (elapsed_ms / 1000);
            fireballMotion.velocity.y += (direction.y) * FIREBALL_ACCELERATION * (elapsed_ms / 1000);
        }
    }
}

void WorldSystem::updateHomingProjectiles(float elapsed_ms) {
    for (Entity entity : registry.homingProjectiles.entities) {
        HomingProjectile& projectile = registry.homingProjectiles.get(entity);
        Motion& projectileM = registry.motions.get(entity);
        Motion& targetM = registry.motions.get(projectile.targetEntity);

        // calculate direction towards target
        vec3 targetPos = targetM.position; 
        vec3 arrowPos = projectileM.position;   
        vec3 direction = targetPos - arrowPos;
        direction = normalize(direction);

        // set the arrow's velocity towards the target 
        projectileM.velocity.x = direction.x * projectile.speed;
        projectileM.velocity.y = direction.y * projectile.speed;
        projectileM.velocity.z = direction.z * projectile.speed;

        // update position based on velocity
        projectileM.position.x += projectileM.velocity.x * elapsed_ms / 1000.0f; // Convert ms to seconds
        projectileM.position.y += projectileM.velocity.y * elapsed_ms / 1000.0f;
        projectileM.position.z += projectileM.velocity.z * elapsed_ms / 1000.0f;
    }
}

void WorldSystem::soundSetUp() {
    // stop all sounds first
    sound->stopAllSounds();
    // init sound system
    sound->init();
    // play background music
    sound->playMusic(Music::BACKGROUND, -1);
}