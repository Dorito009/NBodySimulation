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
        const glm::vec3& target = glm::vec3(0.0f),
        float distanceToTarget = 10.0f,
        float yaw = 0.0f,
        float pitch = 0.0f,
        float fov = 45.0f,
        float nearClip = 0.01f,
        float farClip = 1000.0f,
        ProjectionType type = ProjectionType::Perspective
    );

    glm::vec3 getPosition() const;
    glm::vec3 getTarget() const;

    glm::mat4& getViewMatrix();
    glm::mat4& getProjectionMatrix();

    void update(float deltaTime);
    void handleEvent(const SDL_Event& e);
    glm::mat3 getBasisVectors();

    bool getDirty() const {return mViewDirty;};

private:
    Window& mWindow;

    glm::vec3 mTarget;
    float mDistanceToTarget;
    float mYaw;
    float mPitch;

    glm::vec3 mPosition;
    glm::vec3 mForward;
    glm::vec3 mRight;
    glm::vec3 mUp;

    float mFOV;
    float mAspect;
    float mNearClip;
    float mFarClip;
    ProjectionType mProjType;

    glm::mat4 mViewMatrix;
    glm::mat4 mProjMatrix;
    bool mViewDirty;
    bool mProjDirty;

    bool mFirstMouse = true;
    int mLastMouseX = 0, mLastMouseY = 0;

    void updateViewMatrix();
    void updateProjectionMatrix();
};