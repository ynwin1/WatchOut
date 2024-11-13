#include "game_state_controller.hpp"

#include "world_system.hpp"
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
    currentGameState = newGameState;
}

void GameStateController::onExitState(GAME_STATE oldState) {
    // Implement the logic for exiting a state
    if (oldState == GAME_STATE::PAUSED && world) {
        world->exitPauseMenu();
    }
}