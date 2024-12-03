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
	int highScoreHours = 0;
	int highScoreMinutes = 0;
	int highScoreSeconds = 0;
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
    HELP
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

    Entity mouseTextureEntity;
private:
    GAME_STATE currentGameState;
    WorldSystem* world;
};
