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

// The player
Entity createPlayer(RenderSystem* renderer, vec2 pos);

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
