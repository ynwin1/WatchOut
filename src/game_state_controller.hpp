#pragma once

#include "common.hpp"

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

struct GameScore {
	int highScoreHours = 0;
	int highScoreMinutes = 0;
	int highScoreSeconds = 0;
};

enum class GAME_STATE
{
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

	GameTimer gameTimer;
	GameScore gameScore;
private:
    GAME_STATE currentGameState;
    WorldSystem* world;
};
