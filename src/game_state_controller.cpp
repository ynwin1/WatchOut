#include "game_state_controller.hpp"

#include "world_system.hpp"
#include "common.hpp"

GameStateController::GameStateController(GAME_STATE initialGameState, WorldSystem* world)
    :currentGameState(initialGameState), world(world) {}

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