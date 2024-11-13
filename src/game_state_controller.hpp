#pragma once

#include "common.hpp"


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
    GameStateController(GAME_STATE initialGameState, WorldSystem *world);

    void setGameState(GAME_STATE newGameState);

    void onExitState(GAME_STATE oldState);

    const GAME_STATE getGameState() const {
        return currentGameState;
    }

private:
    GAME_STATE currentGameState;
    WorldSystem* world;
};
