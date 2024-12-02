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

	void restart();

	EnemiesKilled enemiesKilled;
	float survivalBonusTimer = 0;
private:
    GAME_STATE currentGameState;
    WorldSystem* world;
};
