#pragma once

#include "common.hpp"

struct EnemiesKilled {
    int killSpanCount = 0;
    float spanCountdown = 1000;
    bool spanCountdownStarted = false;
	Entity comboTextEntity;
    void updateKillSpanCount() {
        killSpanCount++;
        if(!spanCountdownStarted) {
            spanCountdownStarted = true;
        } else {
			// reset countdown to allow chaining kills
			spanCountdown = 1000;
		}
    }
    void updateSpanCountdown(float elapsed_ms) {
        if(spanCountdownStarted) {
            spanCountdown -= elapsed_ms;
        }
    }
	void resetKillSpan() {
		killSpanCount = 0;
		spanCountdown = 1000;
		spanCountdownStarted = false;
	}
};

enum class INVENTORY_ITEM
{
    NONE,
    BOW,
    TRAP,
    PHANTOM_TRAP,
    BOMB
};

struct Inventory
{
    std::unordered_map<INVENTORY_ITEM, int> itemCounts;
    std::unordered_map<INVENTORY_ITEM, Entity> itemCountTextEntities;
    Entity equippedEntity;
    INVENTORY_ITEM equipped = INVENTORY_ITEM::NONE;
    void reset()
    {
        itemCounts.clear();
        equipped = INVENTORY_ITEM::NONE;
    }
};

enum class GAME_STATE
{
    TITLE,
    TITLE_TUTORIAL,
    PLAYING,
    PAUSED,
    GAMEOVER,
    HELP,
    TUTORIAL,
    BOAR_TUTORIAL,
    WIZARD_TUTORIAL,
    ARCHER_TUTORIAL,
    BIRD_TUTORIAL,
    TROLL_TUTORIAL,
    BARBARIAN_TUTORIAL,
    BOMBER_TUTORIAL,
    HEART_TUTORIAL,
    TRAP_TUTORIAL,
    PHANTOM_TRAP_TUTORIAL,
    BOW_TUTORIAL,
    BOMB_TUTORIAL
};

class WorldSystem;

class GameStateController
{
public:
    void init(GAME_STATE initialGameState, WorldSystem *world);

    void setGameState(GAME_STATE newGameState);

    void onExitState(GAME_STATE oldState);

    void beforeEnterState(GAME_STATE newState);

    const GAME_STATE getGameState() const;

	void restart();

	EnemiesKilled enemiesKilled;
	float survivalBonusTimer = 0;
    Entity mouseTextureEntity;
private:
    GAME_STATE currentGameState;
    WorldSystem* world;
};
