#pragma once

#include "common.hpp"


enum class GAME_STATE
{
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
    HEART_TUTORIAL,
    TRAP_TUTORIAL
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

private:
    GAME_STATE currentGameState;
    WorldSystem* world;
};
