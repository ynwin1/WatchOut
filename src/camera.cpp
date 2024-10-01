#include "camera.hpp"

Camera::Camera() 
{
    // set bounds vectors
    float halfWidth = width / 2;
    float halfHeight = height / 2;
    boundsMin.x = halfWidth;
    boundsMax.x = world_size_x - halfWidth;
    boundsMin.y = halfHeight;
    boundsMax.y = world_size_y - halfHeight;
}

void Camera::followPosition(vec2 newPosition) {
    position = newPosition;

    // Clamp camera position to the bounds
    position.x = glm::clamp(position.x, boundsMin.x, boundsMax.x);
    position.y = glm::clamp(position.y, boundsMin.y, boundsMax.y);
}

vec2 Camera::getPosition() const {
    return position;
}

float Camera::getWidth() const {
    return width;
}

float Camera::getHeight() const {
    return height;
}
