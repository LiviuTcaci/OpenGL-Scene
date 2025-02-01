#include "Camera.hpp"

namespace gps {
    void Camera::setPosition(const glm::vec3& position) {
        cameraPosition = position;
        // Actualizează cameraTarget sau direcția frontala dacă e necesar
        cameraTarget = cameraPosition + cameraFrontDirection;
    }

    void Camera::setTarget(const glm::vec3& target) {
        cameraTarget = target;
        // Actualizează și direcția frontala dacă e necesar:
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    }
    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;

        this->cameraUpDirection = cameraUp;
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));

    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
    }
    glm::vec3 Camera::get_position() {
        return this->cameraPosition;
    }
    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        if (direction == MOVE_FORWARD)
            this->cameraPosition += cameraFrontDirection * speed;
        if (direction == MOVE_BACKWARD)
            this->cameraPosition -= cameraFrontDirection * speed;
        if (direction == MOVE_RIGHT)
            this->cameraPosition += cameraRightDirection * speed;
        if (direction == MOVE_LEFT)
            this->cameraPosition -= cameraRightDirection * speed;
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {

        glm::vec3 aux; //pitch -89 _ 79
        float pitch1;
        if (pitch < -89.0f)
            pitch1 = -89.0f;
        else if (pitch > 79.0f)
            pitch1 = 79.0f;
        else
            pitch1 = pitch;

        aux.x = glm::cos(glm::radians(pitch1)) * glm::cos(glm::radians(yaw));
        aux.y = glm::sin(glm::radians(pitch1));
        aux.z = glm::cos(glm::radians(pitch1)) * glm::sin(glm::radians(yaw));
        this->cameraFrontDirection = glm::normalize(aux);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));


    }
}