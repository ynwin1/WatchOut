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
    auto mouse_button_redirect = [](GLFWwindow* wnd, int button, int action, int mods) {((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_button(button, action, mods); };
    glfwSetKeyCallback(window, key_redirect);
    glfwSetCursorPosCallback(window, cursor_pos_redirect);
    glfwSetMouseButtonCallback(window, mouse_button_redirect);
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

    gameStateController.restart();
    registry.gameScore.score = 0;
    show_mesh = false;
    resetSpawnSystem();
    initText();
    soundSetUp();

    // Set spawn delays to 1 second, so the first of each type will spawn right away
    for (auto& name : entity_types) {
        next_spawns[name] = 1000;
    }
    loadAndSaveHighScore(false);

    tutorialDelayTimer = 0.0f;
    hasSwitchedToTutorial = false;
    encounteredEnemies.clear();
    encounteredCollectibles.clear();
}

void WorldSystem::load_game() {
    if (!saveManager->load_game()) {
		return;
    }
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

void WorldSystem::handleSurvivalBonusPoints(float elapsed_ms) {
    gameStateController.survivalBonusTimer += elapsed_ms;
    if (gameStateController.survivalBonusTimer >= SURVIVAL_BONUS_INTERVAL) {
        int points = 35;
        registry.gameScore.score += points;
        createPointsEarnedText("SURVIVAL BONUS +" + std::to_string(points), playerEntity, {0.8f, 0.8f, 0.0f, 1.0f}, -130.0f);
        gameStateController.survivalBonusTimer = 0;
    }
}

void WorldSystem::initText() {
    createPauseHelpText(camera->getSize());
    registry.fpsTracker.textEntity = createFPSText(camera->getSize());
    registry.gameTimer.reset();
    registry.gameTimer.textEntity = createGameTimerText(camera->getSize());
    registry.gameScore.textEntity = createScoreText(camera->getSize());

    registry.inventory.reset();
    std::unordered_map<INVENTORY_ITEM, Entity>& itemCountTextEntities = registry.inventory.itemCountTextEntities;
    itemCountTextEntities[INVENTORY_ITEM::BOW] = createItemCountText(camera->getSize(), TEXTURE_ASSET_ID::BOW);
    itemCountTextEntities[INVENTORY_ITEM::BOMB] = createItemCountText(camera->getSize(), TEXTURE_ASSET_ID::BOMB);
    trapsCounter.reset();

    // init trapsCounter with text
	trapsCounter.trapsMap[DAMAGE_TRAP] = { 0, createItemCountText(camera->getSize(), TEXTURE_ASSET_ID::TRAPCOLLECTABLE) };
	trapsCounter.trapsMap[PHANTOM_TRAP] = { 0, createItemCountText(camera->getSize(), TEXTURE_ASSET_ID::PHANTOM_TRAP_BOTTLE_ONE) };
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

void WorldSystem::updateInventoryItemText() {
    Inventory& inventory = registry.inventory;
    for (auto& item : inventory.itemCountTextEntities) {
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

void WorldSystem::updateTutorial(float elapsed_ms) {
    if (!hasSwitchedToTutorial) {
        tutorialDelayTimer += elapsed_ms / 1000.0f; 
        if (tutorialDelayTimer >= 0.1f) {
            gameStateController.setGameState(GAME_STATE::TUTORIAL);
            hasSwitchedToTutorial = true;
        }
    }
}

void WorldSystem::updateEnemyTutorial() {
    for(Entity enemy: registry.enemies.entities){
        Motion& motion = registry.motions.get(enemy);
        Motion& playerMotion = registry.motions.get(playerEntity);
        vec2 playerPosition = { playerMotion.position.x, playerMotion.position.y };

        vec2 enemyPosition = { motion.position.x, motion.position.y };
        float distance = glm::distance(playerPosition, enemyPosition);

         if (distance <= 300.0f) {
            std::string enemyType = registry.enemies.get(enemy).type; 
            if (encounteredEnemies.find(enemyType) == encounteredEnemies.end()) {
                createTutorialTarget(motion.position);
                if (enemyType == "BOAR") {
                    gameStateController.setGameState(GAME_STATE::BOAR_TUTORIAL);
                }
                if (enemyType == "BIRD") {
                    gameStateController.setGameState(GAME_STATE::BIRD_TUTORIAL);
                }
                if (enemyType == "TROLL") {
                    gameStateController.setGameState(GAME_STATE::TROLL_TUTORIAL);
                }
                if (enemyType == "WIZARD") {
                    gameStateController.setGameState(GAME_STATE::WIZARD_TUTORIAL);
                }
                if (enemyType == "ARCHER") {
                    gameStateController.setGameState(GAME_STATE::ARCHER_TUTORIAL);
                }
                if (enemyType == "BARBARIAN") {
                    gameStateController.setGameState(GAME_STATE::BARBARIAN_TUTORIAL);
                }
                if (enemyType == "BOMBER") {
                    gameStateController.setGameState(GAME_STATE::BOMBER_TUTORIAL);
                }
                
                encounteredEnemies.insert(enemyType);
                break; 
            }
        }
    }
}

void WorldSystem::updateCollectibleTutorial() {
    for(Entity collectible: registry.collectibles.entities){
        Motion& motion = registry.motions.get(collectible);
        Motion& playerMotion = registry.motions.get(playerEntity);
        vec2 playerPosition = { playerMotion.position.x, playerMotion.position.y };
       
        vec2 collectiblePosition = { motion.position.x, motion.position.y };
        float distance = glm::distance(playerPosition, collectiblePosition);
        if (distance <= 200.0f) {
            std::string collectibleType = registry.collectibles.get(collectible).type; 
            if (encounteredCollectibles.find(collectibleType) == encounteredCollectibles.end()) {
                createTutorialTarget(motion.position);
                if (collectibleType == "HEART") {
                    gameStateController.setGameState(GAME_STATE::HEART_TUTORIAL);
                }
                if (collectibleType == "TRAP") {
                    gameStateController.setGameState(GAME_STATE::TRAP_TUTORIAL);
                }
                if (collectibleType == "PHANTOM_TRAP") {
                    gameStateController.setGameState(GAME_STATE::PHANTOM_TRAP_TUTORIAL);
                }
                if (collectibleType == "BOW") {
                    gameStateController.setGameState(GAME_STATE::BOW_TUTORIAL);
                }
                if (collectibleType == "BOMB") {
                    gameStateController.setGameState(GAME_STATE::BOMB_TUTORIAL);
                }
                encounteredCollectibles.insert(collectibleType);
                break; 
            }
        }
    }
}
void WorldSystem::updateEquippedPosition() {
	Entity& playerE = registry.players.entities[0];
	Motion& playerM = registry.motions.get(playerE);

    if(registry.motions.has(registry.inventory.equippedEntity)) {
        Motion& equippedM = registry.motions.get(registry.inventory.equippedEntity);
        equippedM.position = playerM.position;

        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);
        vec3 mouseWorldPos = renderer->mouseToWorld({mousePosX, mousePosY});

        const float fixedDistance = abs(playerM.scale.x) / 2;

        vec3 direction = mouseWorldPos - playerM.position;
        vec3 normalizedDirection = normalize(direction);

        equippedM.position = playerM.position + normalizedDirection * fixedDistance;

        if(registry.inventory.equipped == INVENTORY_ITEM::BOW) {
            float angle = atan2(direction.y, direction.x);
            equippedM.angle = angle;
        }
    }
}

bool WorldSystem::step(float elapsed_ms)
{
    updateEnemyTutorial();
    updateCollectibleTutorial();
    updateTutorial(elapsed_ms);
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
    updateInventoryItemText();
    toggleMesh();
    accelerateFireballs(elapsed_ms);
    despawnTraps(elapsed_ms);
    updateCollectedTimer(elapsed_ms);
    resetTrappedEntities();
    handleEnemiesKilledInSpan(elapsed_ms);
    updateScoreText();
    handleSurvivalBonusPoints(elapsed_ms);
    updateHomingProjectiles(elapsed_ms);
    updateEquippedPosition();
    updatePointLightPositions(elapsed_ms);

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

void WorldSystem::handleEnemiesKilledInSpan(float elapsed_ms) {
    EnemiesKilled& enemiesKilled = gameStateController.enemiesKilled;
    enemiesKilled.updateSpanCountdown(elapsed_ms);

    bool shouldShowComboText = enemiesKilled.spanCountdownStarted &&
                               enemiesKilled.killSpanCount > 1 &&
                               !registry.texts.has(enemiesKilled.comboTextEntity);

    if(shouldShowComboText) {
        enemiesKilled.comboTextEntity = createComboText(enemiesKilled.killSpanCount, camera->getSize());
    }

    if(enemiesKilled.spanCountdown <= 0) {
        if(enemiesKilled.killSpanCount > 1) {
            int points;
            if(enemiesKilled.killSpanCount < 5) {
                points = enemiesKilled.killSpanCount * 2;
            } else  {
                points = enemiesKilled.killSpanCount * 5;
            }
            registry.gameScore.score += points;
            createPointsEarnedText("BONUS +" + std::to_string(points), playerEntity, {0.8f, 0.8f, 0.0f, 1.0f}, -60.0f);
        }
        enemiesKilled.resetKillSpan();
    }
}

void WorldSystem::updateComboText() {
    EnemiesKilled& enemiesKilled = gameStateController.enemiesKilled;
    if(registry.texts.has(enemiesKilled.comboTextEntity) && enemiesKilled.killSpanCount > 1) {
        Text& text = registry.texts.get(enemiesKilled.comboTextEntity);
        SlideUp& slideUp = registry.slideUps.get(enemiesKilled.comboTextEntity);
        text.value = "COMBO *" + std::to_string(enemiesKilled.killSpanCount);
        slideUp.animationLength = 1500;
    }
}

void WorldSystem::updateScoreText() {
    GameScore& gameScore = registry.gameScore;
    Text& text = registry.texts.get(gameScore.textEntity);
    text.value = "Score: " + std::to_string(gameScore.score);
}

void WorldSystem::updatePointLightPositions(float elapsed_ms) {
    for (Entity& pointLightEntity: registry.pointLights.entities) {
        // Update Position
        PointLight& pointLight = registry.pointLights.get(pointLightEntity);
        if (registry.motions.has(pointLightEntity)) {
            Motion& motion = registry.motions.get(pointLightEntity);
            pointLight.position = motion.position;
        }
    }
    // TODO: Make flicker
}

void WorldSystem::loadAndSaveHighScore(bool save) {
    std::string filename = "highscore.txt";
    GameTimer& gameTimer = registry.gameTimer;
    GameScore& gameScore = registry.gameScore;
    if (save) {
        bool isNewHighScoreTime = gameTimer.hours > gameScore.highScoreHours || 
                                (gameTimer.hours == gameScore.highScoreHours && gameTimer.minutes > gameScore.highScoreMinutes) ||
                                (gameTimer.hours == gameScore.highScoreHours && gameTimer.minutes == gameScore.highScoreMinutes && gameTimer.seconds > gameScore.highScoreSeconds);
        bool isNewHighScore = gameScore.score > gameScore.highScore;

    if (isNewHighScoreTime) {
        gameScore.highScoreHours = gameTimer.hours;
        gameScore.highScoreMinutes = gameTimer.minutes;
        gameScore.highScoreSeconds = gameTimer.seconds;
    }
    if (isNewHighScore) {
        gameScore.highScore = gameScore.score;
    }

    if (isNewHighScoreTime || isNewHighScore) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << gameScore.highScoreHours << " "
                 << gameScore.highScoreMinutes << " "
                 << gameScore.highScoreSeconds << "\n";
            file << gameScore.highScore;
            file.close();
        }
    }
    } else {
        std::ifstream file(filename);
        if (file.is_open()) {
            file >> gameScore.highScoreHours >> gameScore.highScoreMinutes >> gameScore.highScoreSeconds;
            file >> gameScore.highScore;
            file.close();
        } else {
            //if file doesnt exist (shouldn't be an issue)
            gameScore.highScoreHours = 0;
            gameScore.highScoreMinutes = 0;
            gameScore.highScoreSeconds = 0;
            gameScore.highScore = 0;
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

void WorldSystem::updateMouseTexturePosition(vec2 mouse_position) {
    if(registry.foregrounds.has(gameStateController.mouseTextureEntity)) {
        vec2 screenPos = renderer->mouseToScreen(mouse_position);
        Foreground& fg = registry.foregrounds.get(gameStateController.mouseTextureEntity);
        fg.position = screenPos;
    }
}

void WorldSystem::equipItem(INVENTORY_ITEM item, bool wasCollected) {
    Inventory& inventory = registry.inventory;

    if(item == inventory.equipped || 
    (wasCollected && inventory.equipped != INVENTORY_ITEM::NONE))
    {
        return;
    }

    if(inventory.itemCounts[item] > 0) {
        unEquipItem(); // unequip current item

        inventory.equipped = item;

        // disable mouse cursor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);
        vec2 mousePos = renderer->mouseToScreen({mousePosX, mousePosY});

        gameStateController.mouseTextureEntity = createMousePointer(mousePos);

        switch (inventory.equipped)
        {
        case INVENTORY_ITEM::TRAP:
            inventory.equippedEntity = createEquipped(TEXTURE_ASSET_ID::TRAPCOLLECTABLE);
            break;
        case INVENTORY_ITEM::BOW:
            inventory.equippedEntity = createEquipped(TEXTURE_ASSET_ID::BOW);
            break;
        case INVENTORY_ITEM::PHANTOM_TRAP:
            inventory.equippedEntity = createEquipped(TEXTURE_ASSET_ID::PHANTOM_TRAP_BOTTLE_ONE);
            break;
        case INVENTORY_ITEM::BOMB:
            inventory.equippedEntity = createEquipped(TEXTURE_ASSET_ID::BOMB);
            break;
        default:
            break;
        }
    }
}

void WorldSystem::unEquipItem() {
    registry.inventory.equipped = INVENTORY_ITEM::NONE;
    registry.remove_all_components_of(registry.inventory.equippedEntity);
    registry.remove_all_components_of(gameStateController.mouseTextureEntity);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

// Should the game be over ?
bool WorldSystem::is_over() const {
    return bool(glfwWindowShouldClose(window));
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
    updateMouseTexturePosition(mouse_position);
}

Entity WorldSystem::shootHomingArrow(Entity targetEntity, float angle) {
    Motion& targetM = registry.motions.get(targetEntity);
    Motion& playerM = registry.motions.get(registry.players.entities.at(0));

    // get start position of the arrow
    vec3 normalizedDirection = normalize(vec3(targetM.position) - vec3(playerM.position));
    const float fixedDistance = abs(playerM.scale.x) / 2;
    vec3 pos = playerM.position + normalizedDirection * fixedDistance;

    Entity arrowE = createArrow(pos, vec3(0), PLAYER_ARROW_DAMAGE);
    registry.motions.get(arrowE).angle = angle;
    registry.homingProjectiles.emplace(arrowE, targetEntity).speed = HOMING_ARROW_SPEED;

    return arrowE;
}


void WorldSystem::shootArrow(vec3 mouseWorldPos) {
    vec3 playerPos = registry.motions.get(playerEntity).position;
    Entity arrow;
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
            arrow = shootHomingArrow(birdE, angle);
            break;
        }
    }

    if(!birdClicked) {
        arrow = shootProjectile(mouseWorldPos, PROJECTILE_TYPE::ARROW);
    }

    // add damaging component to projectile
    Damaging& damaging = registry.damagings.emplace(arrow);
    damaging.excludedEntity = playerEntity;
    damaging.damage = PLAYER_ARROW_DAMAGE;
}

Entity WorldSystem::shootProjectile(vec3 targetPos, PROJECTILE_TYPE type) {
    const float ANGLE = M_PI / 4;
    const float MAX_VELOCITY = 10;
    vec2 projectileSize = getProjectileInfo(type).size;

    Motion& motion = registry.motions.get(registry.players.entities.at(0));

    // get start position of the projectile
    vec2 horizontal_direction = normalize(vec2(targetPos) - vec2(motion.position));
    const float maxProjectileDimension = max(projectileSize.x, projectileSize.y);
    vec3 pos = motion.position;
    if (abs(horizontal_direction.x) > abs(horizontal_direction.y)) {
        pos.x += (horizontal_direction.x > 0 ? 1 : -1) * (motion.hitbox.x / 2 + maxProjectileDimension);
    } else {
        pos.y += (horizontal_direction.y > 0 ? 1 : -1) * (motion.hitbox.y / 2 + maxProjectileDimension);
    }
    pos.z += motion.hitbox.z / 2 + maxProjectileDimension;
    horizontal_direction = normalize(vec2(targetPos) - vec2(pos));

    float horizontal_distance = distance(vec2(pos), vec2(targetPos));
    float vertical_distance = targetPos.z - pos.z;

    // prevent trying to shoot above what's possible
    if (vertical_distance >= horizontal_distance)
        vertical_distance = horizontal_distance - 1;

    float horizontal_velocity, vertical_velocity;

    // apply bounce logic if the type is a bomb
    if (type == PROJECTILE_TYPE::BOMB_FUSED) {
        float post_bounce_distance = horizontal_distance / (1 + BOUNCE_FACTOR);
        float pre_bounce_distance = horizontal_distance - post_bounce_distance;

        horizontal_velocity = sqrt(-GRAVITATIONAL_CONSTANT * pre_bounce_distance / 
                                   (tan(ANGLE) * (vertical_distance / pre_bounce_distance - tan(ANGLE))));

        // prevent crazy speeds
        if (horizontal_velocity > MAX_VELOCITY)
            horizontal_velocity = MAX_VELOCITY;

        vertical_velocity = horizontal_velocity * tan(ANGLE);
    } else {
        float velocity = horizontal_distance * sqrt(-GRAVITATIONAL_CONSTANT / (vertical_distance - horizontal_distance));

        // prevent crazy speeds
        if (velocity > MAX_VELOCITY)
            velocity = MAX_VELOCITY;

        horizontal_velocity = velocity * cos(ANGLE);
        vertical_velocity = velocity * sin(ANGLE);
    }

    vec2 horizontal_velocity_vector = horizontal_velocity * horizontal_direction;

    Entity projectile = createProjectile(pos, vec3(horizontal_velocity_vector, vertical_velocity), type);

    if (type == PROJECTILE_TYPE::TRAP || type == PROJECTILE_TYPE::PHANTOM_TRAP) {
        registry.projectiles.get(projectile).sticksInGround = 0;
    } else if (type == PROJECTILE_TYPE::BOMB_FUSED) {
        registry.projectiles.get(projectile).sticksInGround = 500;
    }

    return projectile;
}


void WorldSystem::leftMouseClickAction(vec3 mouseWorldPos) {
    Inventory& inventory = registry.inventory;
    switch(inventory.equipped) {
        case INVENTORY_ITEM::BOW:
            shootArrow(mouseWorldPos);
            inventory.itemCounts[INVENTORY_ITEM::BOW]--;
            break;
        case INVENTORY_ITEM::TRAP:
            shootProjectile(mouseWorldPos, PROJECTILE_TYPE::TRAP);
            inventory.itemCounts[INVENTORY_ITEM::TRAP]--;
            trapsCounter.trapsMap[DAMAGE_TRAP].first = inventory.itemCounts[INVENTORY_ITEM::TRAP];
            break;
        case INVENTORY_ITEM::PHANTOM_TRAP:
            shootProjectile(mouseWorldPos, PROJECTILE_TYPE::PHANTOM_TRAP);
            inventory.itemCounts[INVENTORY_ITEM::PHANTOM_TRAP]--;
            trapsCounter.trapsMap[PHANTOM_TRAP].first = inventory.itemCounts[INVENTORY_ITEM::PHANTOM_TRAP];
            break;
        case INVENTORY_ITEM::BOMB: {
            Entity bomb = shootProjectile(mouseWorldPos, PROJECTILE_TYPE::BOMB_FUSED);
            registry.bombs.emplace(bomb);
            inventory.itemCounts[INVENTORY_ITEM::BOMB]--;
        }
            break;
        default:
            return;
    }

    if(inventory.equipped != INVENTORY_ITEM::BOW) {
        sound->playSoundEffect(Sound::WOOSH, 0);
    } else {
        sound->playSoundEffect(Sound::ARROW, 0);
    }

    if(inventory.itemCounts[inventory.equipped] == 0) {
        unEquipItem();
    } else if(registry.animationControllers.has(inventory.equippedEntity)) {
        if(inventory.equipped == INVENTORY_ITEM::BOW) {
            Entity entity = registry.inventory.equippedEntity;
            AnimationController& ac = registry.animationControllers.get(entity);
            ac.changeState(entity, AnimationState::Attack);
        }
    }
}

void WorldSystem::on_mouse_button(int button, int action, int mod) {
    if (action == GLFW_PRESS) {
        switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT: {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos); // get the current cursor position
            vec3 mouseWorldPos = renderer->mouseToWorld({xpos, ypos});
            leftMouseClickAction(mouseWorldPos);
        }
        break;
        case GLFW_MOUSE_BUTTON_RIGHT: {
            if(registry.inventory.equipped != INVENTORY_ITEM::NONE) {
                unEquipItem();
            }
        }
        break;
        }
    }
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
    case GAME_STATE::TUTORIAL: 
        tutorialControls(key, action, mod);
        break;
    case GAME_STATE::BOAR_TUTORIAL: 
    case GAME_STATE::BIRD_TUTORIAL: 
    case GAME_STATE::WIZARD_TUTORIAL: 
    case GAME_STATE::TROLL_TUTORIAL: 
    case GAME_STATE::ARCHER_TUTORIAL: 
    case GAME_STATE::BARBARIAN_TUTORIAL: 
    case GAME_STATE::BOMBER_TUTORIAL: 
        sound->pauseAllSoundEffects();
        enemyTutorialControls(key, action, mod);
        break;
    case GAME_STATE::HEART_TUTORIAL: 
    case GAME_STATE::TRAP_TUTORIAL: 
    case GAME_STATE::PHANTOM_TRAP_TUTORIAL: 
    case GAME_STATE::BOW_TUTORIAL: 
    case GAME_STATE::BOMB_TUTORIAL: 
        sound->pauseAllSoundEffects();
        collectibleTutorialControls(key, action, mod);
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

void WorldSystem::tutorialControls(int key, int action, int mod) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_Q:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_ENTER:
        case GLFW_KEY_SPACE:
            onTutorialClick();
            break;
        case GLFW_KEY_H:
            gameStateController.setGameState(GAME_STATE::PLAYING);
            sound->resumeAllSoundEffects();
            exitTutorial();
            break;
        }
    }
}

void WorldSystem::enemyTutorialControls(int key, int action, int mod) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_Q:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_ENTER:
        case GLFW_KEY_SPACE:
            gameStateController.setGameState(GAME_STATE::PLAYING);
            sound->resumeAllSoundEffects();
            break;
        case GLFW_KEY_ESCAPE:
            gameStateController.setGameState(GAME_STATE::PAUSED);
            break;
        }
    }
}

void WorldSystem::collectibleTutorialControls(int key, int action, int mod) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_Q:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_ENTER:
        case GLFW_KEY_SPACE:
            gameStateController.setGameState(GAME_STATE::PLAYING);
            sound->resumeAllSoundEffects();
            break;
        case GLFW_KEY_ESCAPE:
            gameStateController.setGameState(GAME_STATE::PAUSED);
            break;
        }
    }
}

void WorldSystem::onTutorialClick() {
    Entity entity = registry.tutorialComponents.entities[0];
    TutorialComponent& tutorial = registry.tutorialComponents.get(entity);


    if (tutorial.tutorialStep >= tutorial.maxTutorialSteps) {
        gameStateController.setGameState(GAME_STATE::PLAYING);
        exitTutorial();
        return;
    }
    tutorial.tutorialStep++;
    for (auto& entity : registry.tutorialComponents.entities) {
        TEXTURE_ASSET_ID nextTexture;
        switch (tutorial.tutorialStep) {
        case 2:
            nextTexture = TEXTURE_ASSET_ID::TUTORIAL_2;
            break;
        case 3:
            nextTexture = TEXTURE_ASSET_ID::TUTORIAL_3;
            break;
        default:
            nextTexture = TEXTURE_ASSET_ID::TUTORIAL_1;
            break;
        }

        registry.renderRequests.get(entity).used_texture = nextTexture;
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
        case GLFW_KEY_1:
            equipItem(INVENTORY_ITEM::TRAP);
            break;
        case GLFW_KEY_2:
            equipItem(INVENTORY_ITEM::PHANTOM_TRAP);
            break;
        case GLFW_KEY_3:
            equipItem(INVENTORY_ITEM::BOW);
            break;
        case GLFW_KEY_4:
            equipItem(INVENTORY_ITEM::BOMB);
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

    // Tick invulnerables
    for (Entity entity : registry.invulnerables.entities) {
        Invulnerable& invulnerable = registry.invulnerables.get(entity);
        invulnerable.timer -= elapsed_ms;
        if (invulnerable.timer < 0) {
            registry.invulnerables.remove(entity);
            registry.colours.remove(entity);
        }
    }
}

void WorldSystem::handle_deaths(float elapsed_ms) {
    for (auto& deathEntity : registry.deathTimers.entities) {
        DeathTimer& deathTimer = registry.deathTimers.get(deathEntity);
        deathTimer.timer -= elapsed_ms;
        if (deathTimer.timer < 0) {
            if(registry.archers.has(deathEntity)) {
                createCollectible(registry.motions.get(deathEntity).position, TEXTURE_ASSET_ID::BOW);
            }
            else if(registry.bombers.has(deathEntity)) {
                createCollectible(registry.motions.get(deathEntity).position, TEXTURE_ASSET_ID::BOMB);
            }
            else if (registry.motions.has(deathEntity)) {
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
            registry.inventory.itemCounts[INVENTORY_ITEM::TRAP]++;
			trapsCounter.trapsMap[DAMAGE_TRAP].first = registry.inventory.itemCounts[INVENTORY_ITEM::TRAP];
			createCollected(playerM, collectibleM.scale, TEXTURE_ASSET_ID::TRAPCOLLECTABLE);
            equipItem(INVENTORY_ITEM::TRAP, true);
		}
        else if (collectibleTrap.type == PHANTOM_TRAP) {
            registry.inventory.itemCounts[INVENTORY_ITEM::PHANTOM_TRAP]++;
            trapsCounter.trapsMap[PHANTOM_TRAP].first = registry.inventory.itemCounts[INVENTORY_ITEM::PHANTOM_TRAP];
            createCollected(playerM, collectibleM.scale, TEXTURE_ASSET_ID::PHANTOM_TRAP_BOTTLE_ONE);
            equipItem(INVENTORY_ITEM::PHANTOM_TRAP, true);
        }
    }
    else if (registry.hearts.has(entity_other)) {
        unsigned int health = registry.hearts.get(entity_other).health;
        unsigned int addOn = player.health <= 80 ? health : 100 - player.health;
        player.health += addOn;
        createCollected(playerM, collectibleM.scale, TEXTURE_ASSET_ID::HEART);
		printf("Player collected a heart\n");
	}
    else if (registry.bows.has(entity_other)) {
        registry.inventory.itemCounts[INVENTORY_ITEM::BOW] += 5;
        std::cout << "Player collected a bow. Bow count is now " << registry.inventory.itemCounts[INVENTORY_ITEM::BOW] << std::endl;
        createCollected(playerM, collectibleM.scale, TEXTURE_ASSET_ID::BOW);
        equipItem(INVENTORY_ITEM::BOW, true);
		printf("Player collected a bow\n");
	}
    else if (registry.collectibleBombs.has(entity_other)) {
        registry.inventory.itemCounts[INVENTORY_ITEM::BOMB] += 3;
        createCollected(playerM, collectibleM.scale, TEXTURE_ASSET_ID::BOMB);
        equipItem(INVENTORY_ITEM::BOMB, true);
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

    if (registry.players.has(entity) && !registry.invulnerables.has(entity)) {
        // prevent player taking damage from own damaging object
        if(registry.players.has(damaging.excludedEntity)) {
            return;
        }
        // reduce player health
        Player& player = registry.players.get(entity);
        int new_health = player.health - damaging.damage;
        player.health = new_health < 0 ? 0 : new_health;
        was_damaged.push_back(entity);
        setCollisionCooldown(entity_other, entity);
        registry.invulnerables.emplace(entity);
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

    if (!registry.cooldowns.has(enemy) && !registry.invulnerables.has(player)) {
        Player& playerData = registry.players.get(player);
        Enemy& enemyData = registry.enemies.get(enemy);

        int newHealth = playerData.health - enemyData.damage;
        playerData.health = std::max(newHealth, 0);
        was_damaged.push_back(player);
        setCollisionCooldown(enemy, player);
        registry.invulnerables.emplace(player);
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

        registry.gameScore.score += enemyData.points;
        gameStateController.enemiesKilled.updateKillSpanCount();
        createPointsEarnedText("+" + std::to_string(enemyData.points), enemy, {1.0f, 1.0f, 1.0f, 1.0f}, -20.0f);
        updateComboText();

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
		collectible.timer += elapsed_ms;

        if (collectible.timer >= collectible.duration) {
			registry.remove_all_components_of(collectibleEntity);
		} else if(collectible.timer >= collectible.duration / 2) {
            AnimationController& animatedCollectible = registry.animationControllers.get(collectibleEntity);
            if(animatedCollectible.currentState != AnimationState::Fading) {
                animatedCollectible.changeState(collectibleEntity, AnimationState::Fading);
            }
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

void WorldSystem::place_trap(vec3 trapPos, std::string type) {
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
    
        if (player_comp.isRunning && player_comp.isMoving && stamina.stamina > 0) {
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

void WorldSystem::updateHomingProjectiles(float elapsed_ms) {
    for (Entity entity : registry.homingProjectiles.entities) {
        HomingProjectile& projectile = registry.homingProjectiles.get(entity);
        Motion& projectileM = registry.motions.get(entity);

        // check if target entity still exists
        if(!registry.motions.has(projectile.targetEntity)) {
            registry.remove_all_components_of(entity);
            continue;
        }

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