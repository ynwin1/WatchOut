#pragma once

#include "common.hpp"

struct PlayerResourceUI {
    Entity hpMeshEntity;
	Entity hpFrameEntity;
	Entity hpTextEntity;
    Entity staminaMeshEntity;
	Entity staminaFrameEntity;
	Entity staminaTextEntity;
    vec2 hpMaxSize = {150.f, 20.f};
    vec2 staminaMaxSize = {150.f, 20.f};
};

struct GameScore {
    int score = 0;
    int highScore = 0;
	int highScoreHours = 0;
	int highScoreMinutes = 0;
	int highScoreSeconds = 0;
    Entity textEntity;
};

struct GameTimer {
	int hours = 0;
	int minutes = 0;
	int seconds = 0;
	float ms = 0;
	float elapsed = 0;
	Entity textEntity;
	void update(float elapsedTime) {
		ms += elapsedTime;
		elapsed += elapsedTime;
		if(ms >= 1000.f) {
			ms -= 1000;
        	seconds += 1;
    	}
    	if(seconds >= 60) {
        	seconds -= 60;
        	minutes += 1;
    	}
    	if(minutes >= 60) {
        	minutes -= 60;
        	hours += 1;
    	}
	}
	void reset() {
		ms = 0;
		elapsed = 0;
		hours = 0;
		minutes = 0;
		seconds = 0;
    }
};

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
