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
	int score = 0;
	int highScore = 0;
	Entity textEntity;
	void reset() {
		score = 0;
	}
};

struct TrapsCounter {
	int count = 0;
	Entity textEntity;
	void reset() {
		count = 0;
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
	TrapsCounter trapsCounter;
	EnemiesKilled enemiesKilled;
	float survivalBonusTimer = 0;
private:
    GAME_STATE currentGameState;
    WorldSystem* world;
};
