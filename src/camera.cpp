#include "camera.hpp"
void Camera::setBounds() {
    float halfWidth = width / 2;
    float halfHeight = height / 2;
    boundsMin.x = halfWidth;
    boundsMax.x = world_size_x - halfWidth;
    boundsMin.y = halfHeight;
    boundsMax.y = world_size_y - halfHeight;
}

void Camera::init(GLFWwindow* window) {
    int window_width;
    int window_height;
    glfwGetWindowSize(window, &window_width, &window_height);

    this->width = (float)window_width;
    this->height = (float)window_height;

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

float Camera::getWidth() const {
    return width;
}

float Camera::getHeight() const {
    return height;
}
