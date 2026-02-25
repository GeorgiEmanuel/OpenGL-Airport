#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace gps {

    enum MOVE_DIRECTION { MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT, MOVE_UP, MOVE_DOWN };

    class Camera {

    public:
        //Camera constructor
        Camera() = default;
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
        //return the view matrix, using the glm::lookAt() function
        glm::mat4 getViewMatrix();

        glm::vec3 getPosition();
		void setCameraPosition(glm::vec3 cameraPosition, glm::vec3 camereFrontDirection, glm::vec3 cameraUpDirection, 
            glm::vec3 cameraRightDirection, glm::vec3 oldCameraUpDirection, glm::vec3 oldCameraFrontDirection);
        //update the camera internal parameters following a camera move event
        void move(MOVE_DIRECTION direction, float speed);
        //update the camera internal parameters following a camera rotate event
        //yaw - camera rotation around the y axis
        //pitch - camera rotation around the x axis
        void rotate(float pitch, float yaw);

    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraRightDirection;
        glm::vec3 cameraUpDirection;
        glm::vec3 oldCameraFrontDirection;
		glm::vec3 oldCameraUpDirection;
    };
}

#endif /* Camera_hpp */
