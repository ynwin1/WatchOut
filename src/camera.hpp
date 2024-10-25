#pragma once

#include "common.hpp" 

class Camera {
private:
    vec2 position = {0.0f, 0.0f};
    vec2 size = {0.0f, 0.0f};
    float width;
    float height;
    vec2 boundsMin; // Bounds vectors to restrict camera movement within game world
    vec2 boundsMax; 
    bool toggled = true;

public:
    bool toggle();
    bool isToggled();
    void followPosition(vec2 position);
    vec2 getPosition() const; 
    vec2 getSize() const;
    void setBounds();
    void init(GLFWwindow* window);
};
