#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

		this->cameraFrontDirection = glm::normalize(this->cameraTarget - this->cameraPosition);
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
        
        this->oldCameraUpDirection = this->cameraUpDirection;
		this->oldCameraFrontDirection = this->cameraFrontDirection;


    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
    
        if (direction == MOVE_FORWARD) {
			this->cameraPosition += this->cameraFrontDirection * speed;
			this->cameraTarget += this->cameraFrontDirection * speed;
		}
        else if (direction == MOVE_BACKWARD) {
            this->cameraPosition -= this->cameraFrontDirection * speed;
            this->cameraTarget -= this->cameraFrontDirection * speed;
        } 
        else if (direction == MOVE_LEFT) {
            this -> cameraPosition -= this->cameraRightDirection * speed;
            this->cameraTarget -= this->cameraRightDirection * speed;
        } 
        else if (direction == MOVE_RIGHT) {
            this->cameraPosition += this->cameraRightDirection * speed;
            this->cameraTarget += this->cameraRightDirection * speed;
        }
        else if (direction == MOVE_DOWN) {
            this->cameraPosition -= this->cameraUpDirection * speed;
            this->cameraTarget -= this->cameraUpDirection * speed;
        }
        else if (direction == MOVE_UP) {
            this->cameraPosition += this->cameraUpDirection * speed;
            this->cameraTarget += this->cameraUpDirection * speed;
        }

		this->cameraFrontDirection = glm::normalize(this->cameraTarget - this->cameraPosition);
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
    }

    glm::mat4 EulerMatrix (float head, float pitch, float roll) {
        return glm::rotate(glm::mat4(1.0f), glm::radians(head), glm::vec3(0.0f, 1.0f, 0.0f)) *
               glm::rotate(glm::mat4(1.0f), glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
			   glm::rotate(glm::mat4(1.0f), glm::radians(roll), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
		cameraFrontDirection = glm::normalize(glm::vec3(EulerMatrix(yaw, pitch, 0) * glm::vec4(this->oldCameraFrontDirection, 0.0f)));
		cameraRightDirection = glm::normalize(glm::cross(this ->  cameraFrontDirection, this -> oldCameraUpDirection));
		cameraUpDirection = glm::normalize(glm::cross(this -> cameraRightDirection, this->cameraFrontDirection));
		cameraTarget = cameraPosition + cameraFrontDirection;
    }
    void Camera::setCameraPosition(glm::vec3 cameraPosition, glm::vec3 camereFrontDirection, glm::vec3 cameraUpDirection,
        glm::vec3 cameraRightDirection, glm::vec3 oldCameraUpDirection, glm::vec3 oldCameraFrontDirection) {

		this->cameraPosition = cameraPosition;
		this->cameraFrontDirection = camereFrontDirection;
		this->cameraUpDirection = cameraUpDirection;
		this->cameraRightDirection = cameraRightDirection;
		this->oldCameraUpDirection = oldCameraUpDirection;
		this->oldCameraFrontDirection = oldCameraFrontDirection;
		this->cameraTarget = cameraTarget;

          
    }
    glm::vec3 Camera::getPosition() {
        return this->cameraPosition;
	}
}
