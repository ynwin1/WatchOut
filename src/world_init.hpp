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
const float TRAP_COLLECTABLE_BB_WIDTH = 13.f * SPRITE_SCALE;
const float TRAP_COLLECTABLE_BB_HEIGHT = 17.f * SPRITE_SCALE;
const float HEART_BB_WIDTH = 17. * SPRITE_SCALE;
const float HEART_BB_HEIGHT = 15. * SPRITE_SCALE;

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

const float FIREBALL_HITBOX_WIDTH = 10.f * SPRITE_SCALE;
const float FIREBALL_ACCELERATION = 0.5f;

const float BOAR_SPEED = 0.2;
const float BARBARIAN_SPEED = 0.3;
const float ARCHER_SPEED = 0.2;
const float BIRD_SPEED = 0.3;
const float WIZARD_SPEED = 0.1;
const float PLAYER_SPEED = 0.5;
const float TROLL_SPEED = 0.1;

const float JUMP_STAMINA = 20.f;
const float DASH_STAMINA = 30.f;

const int BOAR_HEALTH = 50;
const int BARBARIAN_HEALTH = 30;
const int ARCHER_HEALTH = 25;
const int BIRD_HEALTH = 10;
const int WIZARD_HEALTH = 20;
const int TROLL_HEALTH = 200;

const int BOAR_DAMAGE = 30;
const int BARBARIAN_DAMAGE = 20;
const int ARCHER_DAMAGE = 25;
const int BIRD_DAMAGE = 10;
const int WIZARD_DAMAGE = 50;
const int TROLL_DAMAGE = 5;

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

// The wizard
Entity createWizard(vec2 pos);

Entity createTroll(vec2 pos);

// The collectible trap
Entity createCollectibleTrap(vec2 pos);

// The collectible heart
Entity createHeart(vec2 pos);

// indicator showing the collected item
Entity createCollected(Motion& playerM, vec2 size, TEXTURE_ASSET_ID assetID);

// The damage trap
Entity createDamageTrap(vec2 pos);

// Arrows fired by the archer
Entity createArrow(vec3 pos, vec3 velocity, int damage);

// Fireballs fired by the wizard
Entity createFireball(vec3 pos, vec2 direction);

// Lightning bolt from the sky
Entity createLightning(vec2 pos);

// Playing UI
Entity createPauseHelpText(vec2 windowSize);
Entity createFPSText(vec2 windowSize);
Entity createGameTimerText(vec2 windowSize);
Entity createTrapsCounterText(vec2 windowSize);
Entity createPointsEarned(std::string text, Entity anchoredEntity, vec4 color);

// Game over UI
void createGameOverText(vec2 windowSize);

// GameOver
Entity createGameOver(vec2 pos);

// GameOver
Entity createGameOver(vec2 pos);

// Display bars
void createHealthBar(Entity characterEntity, vec4 color);
void createPlayerHealthBar(Entity characterEntity, vec2 windowSize);
void createPlayerStaminaBar(Entity characterEntity, vec2 windowSize);

// Map objects
void createMapTiles();
Entity createMapTile(vec2 position, vec2 scale);
Entity createObstacle(vec2 position, vec2 scale, TEXTURE_ASSET_ID assetID);
void createObstacles();
Entity createTargetArea(vec3 position, float radius);

// Cliffs
void createCliffs(GLFWwindow* window);
Entity createBottomCliff(vec2 position, vec2 scale);
Entity createSideCliff(vec2 position, vec2 scale);
Entity createTopCliff(vec2 position, vec2 scale);
void createTrees(RenderSystem* renderer);

float getElevation(vec2 xy);
