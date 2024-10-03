#include <iostream>
#include "../src/tiny_ecs_registry.hpp"
#include "../src/tiny_ecs_registry.cpp"
#include "../src/tiny_ecs.hpp"
#include "../src/tiny_ecs.cpp"
#include <glm/glm.hpp> 

// mimic the update_player_movement logic for testing purposes
void test_update_player_movement(Entity playerEntity, float elapsed_ms) {
    PlayerComponent& player_comp = registry.players.get(playerEntity);

    const float speed = player_comp.isRunning ? 2.0f : 1.0f;

    player_comp.position.x += player_comp.velocity.x * speed * elapsed_ms;
    player_comp.position.y += player_comp.velocity.y * speed * elapsed_ms;

    if (player_comp.isDashing) {
        // (dash distance = 4 units (arbitrary can be changed))
        glm::vec2 target_position = player_comp.position + glm::normalize(player_comp.velocity) * 4.0f;

        // Interpolating position
        float t = elapsed_ms / 100.0f; // Arbitrary t (can be changed according to our requirements)
        player_comp.position = glm::mix(player_comp.position, target_position, t); // Using glm::mix for linear interpolation

        // Reset dashing state after dashing
        player_comp.isDashing = false;
    }

    // Reset velocity after applying movement
    player_comp.velocity = glm::vec2(0, 0);
}

//set up and test player movement (dash)
void test_player_movement() {
    // Create an instance of the ECS and add a player component
    Entity playerEntity = Entity(); // Create a new entity for the player
    PlayerComponent player;
    player.position = {0.0f, 0.0f}; // Starting position
    player.velocity = {1.0f, 0.0f}; // Move to the right
    player.isRunning = false; 
    player.isDashing = true; 

    registry.players.insert(playerEntity, player);

    // elapsed time (in milliseconds)
    float elapsed_ms = 10.00f; 

    test_update_player_movement(playerEntity, elapsed_ms);

    PlayerComponent& updatedPlayer = registry.players.get(playerEntity); // Get the updated Player component

    std::cout << "Player position after update: (" 
              << updatedPlayer.position.x << ", " 
              << updatedPlayer.position.y << ")\n";

    glm::vec2 expected_position = {10.4f, 0.0f}; // Expected position after dashing

    if (updatedPlayer.position.x != expected_position.x || updatedPlayer.position.y != expected_position.y) {
        std::cerr << "Test failed! Expected position (" 
                  << expected_position.x << ", " 
                  << expected_position.y << "), got ("
                  << updatedPlayer.position.x << ", "
                  << updatedPlayer.position.y << ")\n";
    } else {
        std::cout << "Test passed!\n";
    }
}

// Main function to run tests
int main() {
    test_player_movement();
    return 0;
}