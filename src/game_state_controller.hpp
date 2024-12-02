#pragma once

#include "common.hpp"

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
    PHANTOM_TRAP_TUTORIAL
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
