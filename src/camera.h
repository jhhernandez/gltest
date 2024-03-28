#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific
// input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for
// use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    bool godMode;

    // constructor with vectors
    Camera(
      glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
      float yaw = YAW,
      float pitch = PITCH
    ) :
      Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY),
      Zoom(ZOOM),
      godMode(false)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(
      float posX,
      float posY,
      float posZ,
      float upX,
      float upY,
      float upZ,
      float yaw,
      float pitch
    ) :
      Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY),
      Zoom(ZOOM),
      godMode(false)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera
    // defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;

        switch(direction) {
          case FORWARD:
            Position += godMode
              ? Front * velocity
              : glm::vec3(Front.x, 0.0f, Front.z) * velocity;
            break;
          case BACKWARD:
            Position -= godMode
              ? Front * velocity
              : glm::vec3(Front.x, 0.0f, Front.z) * velocity;
            break;
          case LEFT:
            Position -= godMode
              ? Right * velocity
              : glm::vec3(Right.x, 0.0f, Right.z) * velocity;
            break;
          case RIGHT:
            Position += godMode
              ? Right * velocity
              : glm::vec3(Right.x, 0.0f, Right.z) * velocity;
            break;
        }
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f) Pitch = 89.0f;
            if (Pitch < -89.0f) Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= yoffset;
        if (Zoom < 1.0f) Zoom = 1.0f;
        if (Zoom > 45.0f) Zoom = 45.0f;
    }

    void toggleGodMode() {
      godMode = !godMode;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front(
          cos(glm::radians(Yaw)) * cos(glm::radians(Pitch)),
          sin(glm::radians(Pitch)),
          sin(glm::radians(Yaw)) * cos(glm::radians(Pitch))
        );
        /*
         * normalize the vectors, because their length gets closer
         * to 0 the more you look up or down which results in slower movement.
         */
        // also re-calculate the Right and Up vector
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};

#endif
