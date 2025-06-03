#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "Window.h"

class Camera {
public:

    enum class ProjectionType { Perspective, Orthographic };

    explicit Camera(
        Window& window,
        const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f,
        float roll = 0.0f,
        float fov = 45.0f,
        float nearClip = 0.01f,
        float farClip = 1000.0f,
        ProjectionType type = ProjectionType::Perspective
    );

    // Get the position of the camera
    glm::vec3 getPosition();
    // Set the position of the camera
    glm::vec3 setPosition(const glm::vec3& position);

    // Get the view matrix
    glm::mat4& getViewMatrix();
    // Get the projection matrix
    glm::mat4& getProjectionMatrix();

    // Update the position of the camera
    void update(float deltaTime);

    // Get the basis vectors of the camera orientation (right, up, forward)
    glm::mat3 getBasisVectors();
private:
    // Internal data
    Window& mWindow;
    glm::vec3 mPosition;
    float     mYaw;
    float     mPitch;
    float     mRoll;

    // Cached basis vectors
    glm::vec3 mForward;
    glm::vec3 mRight;
    glm::vec3 mUp;

    // Projection data
    float mFOV;
    float mAspect;
    float mNearClip;
    float mFarClip;
    ProjectionType mProjType;

    // Cached matrices + dirty flags
    glm::mat4 mViewMatrix;
    glm::mat4 mProjMatrix;
    bool      mViewDirty;
    bool      mProjDirty;


    void updateOrientationVectors();
    void updateViewMatrix();
    void updateProjectionMatrix();
};


