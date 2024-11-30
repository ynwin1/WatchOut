#include "game_state_controller.hpp"

#include "world_system.hpp"
#include "world_init.hpp"
#include "common.hpp"

void GameStateController::init(GAME_STATE initialGameState, WorldSystem* w) {
    currentGameState = initialGameState;
    world = w;
}

const GAME_STATE GameStateController::getGameState() const {
    return currentGameState;
}

void GameStateController::setGameState(GAME_STATE newGameState) {
    onExitState(currentGameState); // Call onExitState before changing the state
    beforeEnterState(newGameState);
    currentGameState = newGameState;
}

void GameStateController::onExitState(GAME_STATE oldState) {
    // Implement the logic for exiting a state
    if (oldState == GAME_STATE::PAUSED) {
        world->exitPauseMenu();
    }
    if (oldState == GAME_STATE::HELP) {
        world->exitHelpMenu();
    }
}

void GameStateController::beforeEnterState(GAME_STATE newState) {
    // Implement the logic before entering a state
    if (newState == GAME_STATE::PAUSED) {
       world->createPauseMenu(world->camera->getSize());
        return;
    }
    if (newState == GAME_STATE::GAMEOVER) {
        createGameOverText(world->camera->getSize());
        registry.slideUps.clear();
        return;
    }
    if (newState == GAME_STATE::HELP) {
        world->createHelpMenu(world->camera->getSize());
        return;
    }
    return;
}

void GameStateController::restart() {
    setGameState(GAME_STATE::PLAYING);
    enemiesKilled.resetKillSpan();
    survivalBonusTimer = 0;
}