#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// hardcoded dimensions of player and enemies (boar, babarian, and archer)
// BB = Bounding Box
const float SPRITE_SCALE = 3.f;

const float BOAR_BB_WIDTH = 28.f * SPRITE_SCALE;
const float BOAR_BB_HEIGHT = 19.f * SPRITE_SCALE;
const float BARBARIAN_BB_WIDTH = 19.f * SPRITE_SCALE;
const float BARBARIAN_BB_HEIGHT = 34.f * SPRITE_SCALE;
const float ARCHER_BB_WIDTH = 32.f * SPRITE_SCALE;
const float ARCHER_BB_HEIGHT = 32.f * SPRITE_SCALE;
const float BIRD_BB_WIDTH = 16.f * SPRITE_SCALE;
const float BIRD_BB_HEIGHT = 8.f * SPRITE_SCALE;
const float WIZARD_BB_WIDTH = 32.f * SPRITE_SCALE;
const float WIZARD_BB_HEIGHT = 32.f * SPRITE_SCALE;
const float TROLL_SIZE_FACTOR = 2.f;
const float TROLL_BB_WIDTH = 48.f * SPRITE_SCALE;
const float TROLL_BB_HEIGHT = 64.f * SPRITE_SCALE;
const float BOMBER_BB_WIDTH = 24.f * SPRITE_SCALE;
const float BOMBER_BB_HEIGHT = 32.f * SPRITE_SCALE;
const float JEFF_BB_WIDTH   = 20.f * SPRITE_SCALE;
const float JEFF_BB_HEIGHT  = 28.f * SPRITE_SCALE;
const float JEFF_RUN_BB_WIDTH   = 24.f * SPRITE_SCALE;
const float JEFF_RUN_BB_HEIGHT  = 32.f * SPRITE_SCALE;
const float GO_BB_WIDTH   = 100.f * SPRITE_SCALE;
const float GO_BB_HEIGHT  = 68.f * SPRITE_SCALE;
const float TREE_BB_HEIGHT = 94 * 2 * SPRITE_SCALE;
const float TREE_BB_WIDTH = 48 * 2 * SPRITE_SCALE;

const float TRAP_BB_WIDTH = 45.f * SPRITE_SCALE;
const float TRAP_BB_HEIGHT = 13.f * SPRITE_SCALE;
const float PHANTOM_TRAP_BB_WIDTH = 20.f * SPRITE_SCALE;
const float PHANTOM_TRAP_BB_HEIGHT = 28.f * SPRITE_SCALE;
const float TRAP_COLLECTABLE_BB_WIDTH = 13.f * SPRITE_SCALE;
const float TRAP_COLLECTABLE_BB_HEIGHT = 17.f * SPRITE_SCALE;
const float PHANTOM_TRAP_COLLECTABLE_BB_WIDTH = 18.f * SPRITE_SCALE;
const float PHANTOM_TRAP_COLLECTABLE_BB_HEIGHT = 18.f * SPRITE_SCALE;
const float HEART_BB_WIDTH = 17. * SPRITE_SCALE;
const float HEART_BB_HEIGHT = 15. * SPRITE_SCALE;
const float BOW_BB_WIDTH = 14.f * SPRITE_SCALE;
const float BOW_BB_HEIGHT = 27.f * SPRITE_SCALE;

const float SHRUB_BB_WIDTH = 30.f * SPRITE_SCALE;
const float SHRUB_BB_HEIGHT = 33.f * SPRITE_SCALE;
const float ROCK_BB_WIDTH = 30.f * SPRITE_SCALE;
const float ROCK_BB_HEIGHT = 35.f * SPRITE_SCALE;
const float ARROW_BB_WIDTH = 16.f * SPRITE_SCALE;
const float ARROW_BB_HEIGHT = 7.f * SPRITE_SCALE;
const float FIREBALL_BB_WIDTH = 48.f * SPRITE_SCALE;
const float FIREBALL_BB_HEIGHT = 24.f * SPRITE_SCALE;
const float LIGHTNING_BB_WIDTH = 48.f * SPRITE_SCALE;
const float LIGHTNING_BB_HEIGHT = 400.f * SPRITE_SCALE;
const float BOMB_BB_WIDTH = 17.0f * SPRITE_SCALE;
const float BOMB_BB_HEIGHT = 17.5f * SPRITE_SCALE;
const float BOMB_FUSED_BB_WIDTH = 17.0f * SPRITE_SCALE;
const float BOMB_FUSED_BB_HEIGHT = 18.5f * SPRITE_SCALE;
const float EXPLOSION_BB_WIDTH = 107.0f * SPRITE_SCALE;
const float EXPLOSION_BB_HEIGHT = 107.0f * SPRITE_SCALE;

const float FIREBALL_HITBOX_WIDTH = 10.f * SPRITE_SCALE;
const float FIREBALL_ACCELERATION = 0.5f;

const float BOAR_SPEED = 0.2;
const float BARBARIAN_SPEED = 0.3;
const float ARCHER_SPEED = 0.2;
const float BIRD_SPEED = 0.3;
const float WIZARD_SPEED = 0.1;
const float PLAYER_SPEED = 0.5;
const float TROLL_SPEED = 0.1;
const float BOMBER_SPEED = 0.2;

const float HOMING_ARROW_SPEED = 2.5f;

const float JUMP_STAMINA = 20.f;
const float DASH_STAMINA = 20.f;

const int BOAR_HEALTH = 30;
const int BARBARIAN_HEALTH = 20;
const int ARCHER_HEALTH = 20;
const int BIRD_HEALTH = 10;
const int WIZARD_HEALTH = 20;
const int TROLL_HEALTH = 200;
const int BOMBER_HEALTH = 100;

const int BOAR_DAMAGE = 20;
const int BARBARIAN_DAMAGE = 20;
const int ARCHER_DAMAGE = 25;
const int BIRD_DAMAGE = 10;
const int WIZARD_DAMAGE = 30;
const int TROLL_DAMAGE = 5;
const int BOMBER_DAMAGE = 0;

const int PLAYER_ARROW_DAMAGE = 30;

// Jeff the Player
Entity createJeff(vec2 position);

Entity createTree(RenderSystem* renderer, vec2 position);

// The boar
Entity createBoar(vec2 pos);

// The barbarian
Entity createBarbarian(vec2 pos);

// The archer
Entity createArcher(vec2 pos);

// The birds
Entity createBirdFlock(vec2 pos);
Entity createBird(vec2 pos);

// The wizard
Entity createWizard(vec2 pos);

Entity createTroll(vec2 pos);

Entity createCollectible(vec2 pos, TEXTURE_ASSET_ID assetID);
Entity createBomber(vec2 pos);

// The collectible trap
Entity createCollectibleTrap(vec2 pos);

// The collectible heart
Entity createHeart(vec2 pos);

// indicator showing the collected item
Entity createCollected(Motion& playerM, vec2 size, TEXTURE_ASSET_ID assetID);

Entity createEquipped(TEXTURE_ASSET_ID assetId);

// The damage trap
Entity createDamageTrap(vec2 pos);

Entity createPhantomTrap(vec2 pos);

// Arrows fired by the archer/player
Entity createArrow(vec3 pos, vec3 velocity, int damage);

// Bombs thrown by the bomber
Entity createBomb(vec3 pos, vec3 velocity);
void createExplosion(vec3 position);

// Fireballs fired by the wizard
Entity createFireball(vec3 pos, vec2 direction);

// Lightning bolt from the sky
Entity createLightning(vec2 pos);

// TitleScreen UI
Entity createTitleScreenBackground(vec2 windowSize);
Entity createTitleScreenTitle(vec2 windowSize);
Entity createTitleScreenText(vec2 windowSize, std::string value, float fontSize, vec2 position);

// Playing UI
Entity createPauseHelpText(vec2 windowSize);
Entity createFPSText(vec2 windowSize);
Entity createGameTimerText(vec2 windowSize);
Entity createPointsEarnedText(std::string text, Entity anchoredWorldEntity, vec4 color, float xOffset);
Entity createComboText(int comboValue, vec2 windowSize);
Entity createScoreText(vec2 windowSize);
Entity createItemCountText(vec2 windowSize, TEXTURE_ASSET_ID assetID);

Entity createMousePointer(vec2 mousePos);
Entity createProjectile(vec3 pos, vec3 velocity, PROJECTILE_TYPE type);

// Game over UI
void createGameOverText(vec2 windowSize);

// Game save text
void createGameSaveText(vec2 windowSize);

// Display bars
void createHealthBar(Entity characterEntity);
void createPlayerHealthBar(Entity characterEntity, vec2 windowSize);
void createPlayerStaminaBar(Entity characterEntity, vec2 windowSize);

// Map objects
void createMapTiles();
Entity createMapTile(vec2 position, vec2 scale, float height);
Entity createObstacle(vec2 position, vec2 scale, TEXTURE_ASSET_ID assetID);
Entity createNormalObstacle(vec2 position, vec2 size, TEXTURE_ASSET_ID assetId);
void createObstacles();
Entity createTargetArea(vec3 position);

// Cliffs
void createCliffs(GLFWwindow* window);
Entity createBottomCliff(vec2 position, vec2 scale);
Entity createSideCliff(vec2 position, vec2 scale);
Entity createTopCliff(vec2 position, vec2 scale);
void createTrees(RenderSystem* renderer);

//Tutorial
Entity createTutorialTarget(vec3 position);
struct ProjectileInfo {
    vec2 size;
    TEXTURE_ASSET_ID assetId;
};
ProjectileInfo getProjectileInfo(PROJECTILE_TYPE type);

float getElevation(vec2 xy);
