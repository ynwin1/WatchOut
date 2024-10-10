#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "common.hpp" 

class Camera {
private:
    vec2 position = {0.0f, 0.0f};
    float width = 975;  // Width and height to determine viewport of camera  
    float height = 548;  
    vec2 boundsMin; // Bounds vectors to restrict camera movement within game world
    vec2 boundsMax; 
    bool toggled = true;

public:
    Camera();
    bool toggle();
    bool isToggled();
    void followPosition(vec2 position);
    vec2 getPosition() const; 
    float getWidth() const;   
    float getHeight() const;  
};

#endif 
