#include "world_system.hpp"
#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "physics_system.hpp"
#include "sound_system.hpp"
#include "game_state_controller.hpp"
#include "game_save_manager.hpp"
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

void WorldSystem::init(
    RenderSystem* renderer, 
    GLFWwindow* window, 
    Camera* camera, 
    PhysicsSystem* physics, 
    AISystem* ai, 
    SoundSystem* sound, 
    GameSaveManager* saveManager, 
    ParticleSystem* particles)
{
    
    this->renderer = renderer;
    this->window = window;
    this->camera = camera;
    this->physics = physics;
    this->ai = ai;
	this->sound = sound;
	this->saveManager = saveManager;
    this->particles = particles;

    // Setting callbacks to member functions (that's why the redirect is needed)
    // Input is handled using GLFW, for more info see
    // http://www.glfw.org/docs/latest/input_guide.html
    glfwSetWindowUserPointer(window, this);
    auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
    auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
    glfwSetKeyCallback(window, key_redirect);
    glfwSetCursorPosCallback(window, cursor_pos_redirect);
    // Window focus callback
    auto focus_redirect = [](GLFWwindow* wnd, int focused) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_window_focus(focused); };
    glfwSetWindowFocusCallback(window, focus_redirect);

	createTitleScreen();
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

    // Set spawn delays to 1 second, so the first of each type will spawn right away
    for (auto& name : entity_types) {
        next_spawns[name] = 1000;
    }
    loadAndSaveHighScore(false);
}

void WorldSystem::load_game() {
    saveManager->load_game();
    saveManager->loadTrapsCounter(trapsCounter.trapsMap);
    // set up texts in foreground
    reloadText();
  
    // Pick up spawn data from last checkpoint
    next_spawns = saveManager->getNextSpawns();
	spawn_delays = saveManager->getSpawnDelays();
	max_entities = saveManager->getMaxEntities();

    show_mesh = false;
    playerEntity = registry.players.entities[0];
    gameStateController.setGameState(GAME_STATE::PLAYING);
    loadAndSaveHighScore(false);
}

void WorldSystem::save_game() {
    std::string filename = "game.txt";
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
    trapsCounter.reset();

    // init trapsCounter with text
	trapsCounter.trapsMap[DAMAGE_TRAP] = { 0, createTrapsCounterText(camera->getSize()) };
	trapsCounter.trapsMap[PHANTOM_TRAP] = { 0, createPhantomTrapsCounterText(camera->getSize()) };
}

void WorldSystem::reloadText() {
    createPauseHelpText(camera->getSize());
    registry.fpsTracker.textEntity = createFPSText(camera->getSize());
    registry.gameTimer.textEntity = createGameTimerText(camera->getSize());
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
    int damageTrapCount = trapsCounter.trapsMap["trap"].first;
    Entity& damageTrapTextEntity = trapsCounter.trapsMap["trap"].second;
    int phantomTrapCount = trapsCounter.trapsMap["phantom_trap"].first;
    Entity& phantomTrapTextEntity = trapsCounter.trapsMap["phantom_trap"].second;

    Text& damageTrapText = registry.texts.get(damageTrapTextEntity);
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << damageTrapCount;
    damageTrapText.value = "*" + ss.str();

	Text& phantomTrapText = registry.texts.get(phantomTrapTextEntity);
	std::stringstream ss2;
	ss2 << std::setw(2) << std::setfill('0') << phantomTrapCount;
	phantomTrapText.value = "*" + ss2.str();

    // Damage Trap
    if(damageTrapCount == 0) {
        registry.colours.get(damageTrapTextEntity) = {0.8f, 0.8f, 0.0f, 1.0f};
    } else {
        registry.colours.get(damageTrapTextEntity) = {1.0f, 1.0f, 1.0f, 1.0f};
    }

	// Phantom Trap
	if (phantomTrapCount == 0) {
		registry.colours.get(phantomTrapTextEntity) = { 0.8f, 0.8f, 0.0f, 1.0f };
	}
	else {
		registry.colours.get(phantomTrapTextEntity) = { 1.0f, 1.0f, 1.0f, 1.0f };
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

bool WorldSystem::step(float elapsed_ms)
{
    adjustSpawnSystem(elapsed_ms);
    spawn(elapsed_ms);
    spawn_particles(elapsed_ms);
    update_cooldown(elapsed_ms);
    handle_deaths(elapsed_ms);
    despawn_collectibles(elapsed_ms);
	destroyDamagings();
    handle_stamina(elapsed_ms);
    trackFPS(elapsed_ms);
    updateGameTimer(elapsed_ms);
    updateTrapsCounterText();
    toggleMesh();
    accelerateFireballs(elapsed_ms);
    despawnTraps(elapsed_ms);
    updateCollectedTimer(elapsed_ms);
    resetTrappedEntities();
    updateJeffLight(elapsed_ms);

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

void WorldSystem::updateJeffLight(float elapsed_ms) {
    PointLight& pointLight = registry.pointLights.get(playerEntity);
    
    // Update Position
    Motion& motion = registry.motions.get(playerEntity);
    pointLight.position = motion.position;

    // Make flicker
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

        std::pair<int, int> pair = { entity, entity_other };
        if (collisionCooldowns.find(pair) != collisionCooldowns.end()) {
            continue;
        }

        // If the entity is a player
        if (registry.players.has(entity)) {
            // If the entity is colliding with a collectible
            if (registry.collectibles.has(entity_other)) {
				entity_collectible_collision(entity, entity_other);
            }
        }
        else if (registry.enemies.has(entity)) {
            if (registry.players.has(entity_other)) {
                // Collision between player and enemy
                processPlayerEnemyCollision(entity_other, entity, was_damaged);
            }
            else if (registry.enemies.has(entity_other)) {
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
            if (registry.players.has(entity_other) || registry.enemies.has(entity_other)) {
                entity_damaging_collision(entity_other, entity, was_damaged);
            }
            else if (damaging.type == "fireball" && registry.obstacles.has(entity_other)) {
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

void WorldSystem::on_key(int key, int, int action, int mod)
{
    switch (gameStateController.getGameState()) {
	case GAME_STATE::TITLE:
		titleControls(key, action, mod);
		break;
    case GAME_STATE::PLAYING:
        playingControls(key, action, mod);
        break;
    case GAME_STATE::PAUSED:
        handleSoundOnPauseHelp();
        pauseControls(key, action, mod);
        break;
    case GAME_STATE::GAMEOVER:
        gameOverControls(key, action, mod);
        break;
    case GAME_STATE::HELP:
        handleSoundOnPauseHelp();
        helpControls(key, action, mod);
        break;
    }
    if (gameStateController.getGameState() != GAME_STATE::TITLE) {
        movementControls(key, action, mod);
    }
    allStateControls(key, action, mod);
}

void WorldSystem::helpControls(int key, int action, int mod)
{
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_Q:
            gameStateController.setGameState(GAME_STATE::TITLE);
            createTitleScreen();
            break;
        case GLFW_KEY_R:
            restart_game();
        case GLFW_KEY_H:
            sound->resumeAllSoundEffects();
            gameStateController.setGameState(GAME_STATE::PLAYING);
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
            gameStateController.setGameState(GAME_STATE::TITLE);
            createTitleScreen();
            break;
        case GLFW_KEY_H:
            gameStateController.setGameState(GAME_STATE::HELP);
            clearSaveText();
            break;
        case GLFW_KEY_S:
            saveManager->save_game(trapsCounter.trapsMap, spawn_delays, max_entities, next_spawns);
			createGameSaveText(camera->getSize());
            printf("Saved game\n");
            break;
        case GLFW_KEY_L:
            load_game();
			  break;
        case GLFW_KEY_ENTER:
            restart_game();
        case GLFW_KEY_P:
        case GLFW_KEY_ESCAPE:
            sound->resumeAllSoundEffects();
            gameStateController.setGameState(GAME_STATE::PLAYING);
            clearSaveText();
            break;
        }
    }
}

void WorldSystem::titleControls(int key, int action, int mod) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ENTER:
			restart_game();
			break;
		case GLFW_KEY_L:
			load_game();
			break;
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, true);
			break;
		}
	}
}

void WorldSystem::playingControls(int key, int action, int mod)
{
    Player& player_comp = registry.players.get(playerEntity);
    Motion& player_motion = registry.motions.get(playerEntity);
    Dash& player_dash = registry.dashers.get(playerEntity);
    Stamina& player_stamina = registry.staminas.get(playerEntity);
  
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_X:
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
            break;
        case GLFW_KEY_E:
            place_trap(player_comp, player_motion, true, DAMAGE_TRAP);
            break;
        case GLFW_KEY_Q:
            place_trap(player_comp, player_motion, false, DAMAGE_TRAP);
            break;
		case GLFW_KEY_L:
			place_trap(player_comp, player_motion, true, PHANTOM_TRAP);
			break;
		case GLFW_KEY_K:
			place_trap(player_comp, player_motion, false, PHANTOM_TRAP);
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
        case GLFW_KEY_R:
            restart_game();
            break;
        case GLFW_KEY_Q:
        case GLFW_KEY_ENTER:
        case GLFW_KEY_ESCAPE:
            gameStateController.setGameState(GAME_STATE::TITLE);
            createTitleScreen();
        }
    }
}

void WorldSystem::allStateControls(int key, int action, int mod)
{
    if (action == GLFW_PRESS) {
        switch (key) {
#ifndef NDEBUG
        case GLFW_KEY_C:
            // toggle camera on/off for debugging/testing
            camera->toggle();
            break;
#endif
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
    Stamina& player_stamina = registry.staminas.get(playerEntity);

    if (action != GLFW_PRESS && action != GLFW_RELEASE) {
        return;
    }

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
    case GLFW_KEY_SPACE:
        // Jump
        player_comp.tryingToJump = pressed;
        break;
    default:
        break;
    }
    update_player_facing(player_comp, player_motion);
}

void WorldSystem::handleSoundOnPauseHelp() {
    sound->isMovingSoundPlaying = false;
    sound->isBirdFlockSoundPlaying = false;
    sound->stopSoundEffect(Sound::BIRD_FLOCK);
    sound->stopSoundEffect(Sound::WALKING);
    sound->pauseAllSoundEffects();
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

	for (Entity& trapE : registry.phantomTraps.entities) {
		PhantomTrap& trap = registry.phantomTraps.get(trapE);
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

void WorldSystem::spawn_particles(float elapsed_ms)
{
    Motion& playerMotion = registry.motions.get(playerEntity);

    // SPAWN SMOKE ------------------------------------------------
    vec3 position = playerMotion.position;
    float direction = (playerMotion.scale.x > 0) ? 1.f : -1.f;
    position.x += direction * playerMotion.hitbox.x / 2;
    position.z += playerMotion.hitbox.z / 3;
    vec2 size = { 20, 20 };
    particles->createSmokeParticle(position, size);
    particles->createSmokeParticle(position, size);
    particles->createSmokeParticle(position, size);

    for (Entity fireball : registry.damagings.entities) {
        Damaging& damaging = registry.damagings.get(fireball);
        if (damaging.type != "fireball") {
            continue;
        }
        vec3 position = registry.motions.get(fireball).position;
        vec2 size = { 50, 50 };
        particles->createSmokeParticle(position, size);
        particles->createSmokeParticle(position, size);
        particles->createSmokeParticle(position, size);
        particles->createSmokeParticle(position, size);
    }

    // SPAWN DASH SPRITES ------------------------------------------------
    if (registry.dashers.get(playerEntity).isDashing) {
        float facing = playerMotion.scale.x > 0 ? 1 : -1;
        particles->createDashParticle(playerMotion.position, vec2(JEFF_BB_WIDTH * facing, JEFF_BB_HEIGHT));
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
		CollectibleTrap& collectibleTrap = registry.collectibleTraps.get(entity_other);
        if (collectibleTrap.type == DAMAGE_TRAP) {
			trapsCounter.trapsMap[DAMAGE_TRAP].first++;
			createCollected(playerM, collectibleM.scale, TEXTURE_ASSET_ID::TRAPCOLLECTABLE);
		}
        else if (collectibleTrap.type == PHANTOM_TRAP) {
            trapsCounter.trapsMap[PHANTOM_TRAP].first++;
            createCollected(playerM, collectibleM.scale, TEXTURE_ASSET_ID::PHANTOM_TRAP_BOTTLE_ONE);
        }
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

    if(registry.knockers.has(entity_other)) {
        knock(entity, entity_other);
    }

    if (registry.players.has(entity)) {
        // reduce player health
        Player& player = registry.players.get(entity);
        int new_health = player.health - damaging.damage;
        player.health = new_health < 0 ? 0 : new_health;
        was_damaged.push_back(entity);
        setCollisionCooldown(entity_other, entity);
        printf("Player health reduced from %d to %d\n", player.health + damaging.damage, player.health);
    }
    else if (registry.enemies.has(entity)) {
        // reduce enemy health
        Enemy& enemy = registry.enemies.get(entity);
        enemy.health -= damaging.damage;
        was_damaged.push_back(entity);
        setCollisionCooldown(entity_other, entity);
        printf("Enemy health reduced from %d to %d by damaging object\n", enemy.health + damaging.damage, enemy.health);
    }
    else {
        printf("Entity is not a player or enemy\n");
        return;
    }

    if(!registry.explosions.has(entity_other) && 
       !registry.bombs.has(entity_other)) 
    {
        registry.remove_all_components_of(entity_other);
    }
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
    // Archers/Bombers/Wizards do not do melee damage
    if (registry.archers.has(enemy) || registry.bombers.has(enemy) || registry.wizards.has(enemy)) {
        return;
    }

    if (!registry.cooldowns.has(enemy)) {
        Player& playerData = registry.players.get(player);
        Enemy& enemyData = registry.enemies.get(enemy);

        int newHealth = playerData.health - enemyData.damage;
        playerData.health = std::max(newHealth, 0);
        was_damaged.push_back(player);
        setCollisionCooldown(enemy, player);
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
        setCollisionCooldown(attacker, target);
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
    registry.knockables.get(knocked).knocked = true;
}

void WorldSystem::setCollisionCooldown(Entity damager, Entity victim)
{
    float COOLDOWN_TIME = 1000;
    collisionCooldowns[std::pair<Entity, Entity> {damager, victim}] = COOLDOWN_TIME;
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
        if(registry.bombs.has(damagingEntity) || registry.explosions.has(damagingEntity)) {
            continue;
        }

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

void WorldSystem::place_trap(Player& player, Motion& motion, bool forward, std::string type) {
    // Player position
    vec2 playerPos = motion.position;
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

	if (type == DAMAGE_TRAP) {
		int trapCount = trapsCounter.trapsMap[DAMAGE_TRAP].first;
		if (trapCount == 0) {
			printf("Player has no damage traps to place\n");
			return;
		}
        createDamageTrap(trapPos);
		trapsCounter.trapsMap[DAMAGE_TRAP].first--;
        printf("Damage trap count is now %d\n", trapsCounter.trapsMap[DAMAGE_TRAP].first);
	}
	else if (type == PHANTOM_TRAP) {
		int trapCount = trapsCounter.trapsMap[PHANTOM_TRAP].first;
		if (trapCount == 0) {
			printf("Player has no phantom traps to place\n");
			return;
		}
		createPhantomTrap(trapPos);
		trapsCounter.trapsMap[PHANTOM_TRAP].first--;
		printf("Phantom trap count is now %d\n", trapsCounter.trapsMap[PHANTOM_TRAP].first);
	}
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
                    EFFECT_ASSET_ID::TEXTURED_NORMAL,
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

void WorldSystem::soundSetUp() {
    // stop all sounds first
    sound->stopAllSounds();
    // init sound system
    sound->init();
    // play background music
    sound->playMusic(Music::BACKGROUND, -1);
}

void WorldSystem::clearSaveText() {
    for (Entity textE : registry.texts.entities) {
        if (registry.cooldowns.has(textE)) {
            registry.remove_all_components_of(textE);
        }
    }
}