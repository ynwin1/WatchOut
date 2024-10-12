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
const float TRAP_BB_WIDTH = 0.3f;
const float TRAP_BB_HEIGHT = 0.2f;
const float JEFF_BB_WIDTH   = 20.f * SPRITE_SCALE;
const float JEFF_BB_HEIGHT  = 28.f * SPRITE_SCALE;
const float GO_BB_WIDTH   = 100.f * SPRITE_SCALE;
const float GO_BB_HEIGHT  = 68.f * SPRITE_SCALE;

const std::unordered_map<std::string, vec2> entity_sizes = {
	{"boar", { BOAR_BB_WIDTH, BOAR_BB_HEIGHT }},
	{"barbarian", { BARBARIAN_BB_WIDTH, BARBARIAN_BB_HEIGHT }},
	{"archer", { ARCHER_BB_WIDTH, ARCHER_BB_HEIGHT }}
};

// Jeff the Player
Entity createJeff(RenderSystem* renderer, vec2 position);

// The boar
Entity createBoar(RenderSystem* renderer, vec2 pos);

// The barbarian
Entity createBarbarian(RenderSystem* renderer, vec2 pos);

// The archer
Entity createArcher(RenderSystem* renderer, vec2 pos);

// The collectible trap
Entity createCollectibleTrap(RenderSystem* renderer, vec2 pos);

// The damage trap
Entity createDamageTrap(RenderSystem* renderer, vec2 pos);

void createBattleGround();

// GameOver
Entity createGameOver(RenderSystem* renderer, vec2 pos);

void createHealthBar(Entity characterEntity);
