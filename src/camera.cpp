#include "camera.hpp"
#include "render_system.hpp"

void Camera::setBounds() {
    float halfWidth = this->size.x / 2;
    float halfHeight = this->size.y / 2;
    boundsMin.x = halfWidth;
    boundsMax.x = world_size_x - halfWidth;
    boundsMin.y = halfHeight;
    boundsMax.y = getVisualYPosition(world_size_y, 0) - halfHeight;
}

void Camera::init(GLFWwindow* window) {
    int window_width;
    int window_height;
    glfwGetWindowSize(window, &window_width, &window_height);

    this->size.x = (float)window_width;
    this->size.y = (float)window_height;

    setBounds();
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

vec2 Camera::getSize() const {
    return size;
}