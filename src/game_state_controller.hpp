#pragma once

#include "common.hpp"

enum class EQUIPPED
{
    NONE,
    TRAP,
    BOW,
};

struct Inventory
{
    int traps = 0;
    int bows = 0;
    Entity equippedEntity;
    EQUIPPED equipped = EQUIPPED::NONE;
    void reset()
    {
        traps = 0;
        bows = 0;
        equipped = EQUIPPED::NONE;
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

    Inventory inventory;

private:
    GAME_STATE currentGameState;
    WorldSystem* world;
};
