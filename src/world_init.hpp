#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// hardcoded dimensions of player and enemies (boar, babarian, and archer)
// BB = Bounding Box
const float PLAYER_BB_WIDTH = 0.5f;
const float PLAYER_BB_HEIGHT = 0.5f;
const float BOAR_BB_WIDTH = 0.8f;
const float BOAR_BB_HEIGHT = 0.4f;
const float BARBARIAN_BB_WIDTH = 0.3f;
const float BARBARIAN_BB_HEIGHT = 0.4f;
const float ARCHER_BB_WIDTH = 0.4f;
const float ARCHER_BB_HEIGHT = 0.5f;
const float TRAP_BB_WIDTH = 0.3f;
const float TRAP_BB_HEIGHT = 0.2f;
const float JEFF_BB_WIDTH   = 0.6f * 300.f;	// 1001
const float JEFF_BB_HEIGHT  = 0.6f * 202.f;	// 870

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
