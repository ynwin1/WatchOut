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
const float JEFF_BB_WIDTH   = 20.f * SPRITE_SCALE;
const float JEFF_BB_HEIGHT  = 28.f * SPRITE_SCALE;
const float GO_BB_WIDTH   = 100.f * SPRITE_SCALE;
const float GO_BB_HEIGHT  = 68.f * SPRITE_SCALE;
const float TREE_BB_HEIGHT = 94 * 2 * SPRITE_SCALE;
const float TREE_BB_WIDTH = 48 * 2 * SPRITE_SCALE;

const float TRAP_BB_WIDTH = 32.f * SPRITE_SCALE;
const float TRAP_BB_HEIGHT = 9.f * SPRITE_SCALE;
const float TRAP_COLLECTABLE_BB_WIDTH = 13.f * SPRITE_SCALE;
const float TRAP_COLLECTABLE_BB_HEIGHT = 17.f * SPRITE_SCALE;
const float HEART_BB_WIDTH = 17. * SPRITE_SCALE;
const float HEART_BB_HEIGHT = 15. * SPRITE_SCALE;

const float SHRUB_BB_WIDTH = 30.f * SPRITE_SCALE;
const float SHRUB_BB_HEIGHT = 33.f * SPRITE_SCALE;
const float ROCK_BB_WIDTH = 30.f * SPRITE_SCALE;
const float ROCK_BB_HEIGHT = 35.f * SPRITE_SCALE;
const float ARROW_BB_WIDTH = 16 * SPRITE_SCALE;
const float ARROW_BB_HEIGHT = 7 * SPRITE_SCALE;

const float BOAR_SPEED = 0.3;
const float BARBARIAN_SPEED = 0.3;
const float ARCHER_SPEED = 0.2;

const std::unordered_map<std::string, vec2> entity_sizes = {
	{"boar", { BOAR_BB_WIDTH, BOAR_BB_HEIGHT }},
	{"barbarian", { BARBARIAN_BB_WIDTH, BARBARIAN_BB_WIDTH }},
	{"archer", { ARCHER_BB_WIDTH, ARCHER_BB_WIDTH }},
	{"heart", { HEART_BB_WIDTH, HEART_BB_WIDTH }},
	{"collectible_trap", {TRAP_COLLECTABLE_BB_WIDTH, TRAP_COLLECTABLE_BB_WIDTH}}
};

// Jeff the Player
Entity createJeff(vec2 position);

Entity createTree(RenderSystem* renderer, vec2 position);

// The boar
Entity createBoar(vec2 pos);

// The barbarian
Entity createBarbarian(vec2 pos);

// The archer
Entity createArcher(vec2 pos);

// The collectible trap
Entity createCollectibleTrap(vec2 pos);

// The collectible heart
Entity createHeart(vec2 pos);

// The damage trap
Entity createDamageTrap(vec2 pos);

// Arrows fired by the archer
Entity createArrow(vec3 pos, vec3 velocity);

Entity createFPSText(vec2 windowSize);
void createGameOverText(vec2 windowSize);
Entity createGameTimerText(vec2 windowSize);

Entity createTrapsCounterText(vec2 windowSize);

// GameOver
Entity createGameOver(vec2 pos);

void createHealthBar(Entity characterEntity, vec3 color);

void createMapTiles();
Entity createMapTile(vec2 position, vec2 scale);
Entity createObstacle(vec2 position, vec2 scale, TEXTURE_ASSET_ID assetID);
void createObstacles();

void createCliffs(GLFWwindow* window);
Entity createCliff(vec2 position, vec2 scale);
Entity createSideCliff(vec2 position, vec2 scale);

void createStaminaBar(Entity characterEntity, vec3 color);

void createTrees(RenderSystem* renderer);

float getElevation(vec2 xy);
