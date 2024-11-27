#pragma once

#include "common.hpp"
#include "tiny_ecs_registry.hpp"
#include "world_init.hpp"

enum class EQUIPMENT
{
    NONE,
    BOW,
    TRAP
};

struct Inventory
{
    std::unordered_map<EQUIPMENT, int> itemCounts;
    Entity equippedEntity;
    EQUIPMENT equipped = EQUIPMENT::NONE;
    void equipItem(EQUIPMENT item)
    {
        if(item == equipped)
        {
            return;
        }
        
        if(itemCounts[item] > 0)
        {
            equipped = item;
            switch (equipped)
            {
            case EQUIPMENT::BOW:
                equippedEntity = createEquipped(TEXTURE_ASSET_ID::BOW, {BOW_BB_WIDTH, BOW_BB_HEIGHT});
                break;
            default:
                break;
            }
        }
    }
    void unequip()
    {
        equipped = EQUIPMENT::NONE;
        registry.remove_all_components_of(equippedEntity);
    }
    void reset()
    {
        itemCounts.clear();
        unequip();
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
