#pragma once
#include "common.hpp"
#include "tiny_ecs_registry.hpp"
#include "world_init.hpp"

enum class INVENTORY_ITEM
{
    NONE,
    BOW,
    TRAP
};

struct Inventory
{
    std::unordered_map<INVENTORY_ITEM, int> itemCounts;
    std::unordered_map<INVENTORY_ITEM, Entity> itemCountTextEntities;
    Entity equippedEntity;
    INVENTORY_ITEM equipped = INVENTORY_ITEM::NONE;
    void equipItem(INVENTORY_ITEM item)
    {
        if(item == equipped)
        {
            return;
        }

        unequip();
        
        if(itemCounts[item] > 0)
        {
            equipped = item;
            switch (equipped)
            {
            case INVENTORY_ITEM::BOW:
                equippedEntity = createEquipped(TEXTURE_ASSET_ID::BOW);
                break;
            default:
                break;
            }
        }
    }
    void equipCollected(INVENTORY_ITEM item)
    {
        if(equipped == INVENTORY_ITEM::NONE)
        {
            equipItem(item);
        }
    }
    void unequip()
    {
        equipped = INVENTORY_ITEM::NONE;
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
