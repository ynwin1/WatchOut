#include "camera.hpp"
#include "render_system.hpp"

Camera::Camera() 
{
    // set bounds vectors
    float halfWidth = width / 2;
    float halfHeight = height / 2;
    boundsMin.x = halfWidth;
    boundsMax.x = world_size_x - halfWidth;
    boundsMin.y = halfHeight;
    boundsMax.y = getVisualYPosition(world_size_y, 0) - halfHeight;
}

void Camera::followPosition(vec2 newPosition) {
    position = newPosition;

    // Clamp camera position to the bounds
    position.x = glm::clamp(position.x, boundsMin.x, boundsMax.x);
    position.y = glm::clamp(position.y, boundsMin.y, boundsMax.y);
}

bool Camera::toggle() {
    toggled = !toggled;
    return toggled;
}

bool Camera::isToggled() {
    return toggled;
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
