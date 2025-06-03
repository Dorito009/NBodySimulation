#include "Camera.h"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

//Todo make this class a lot cleaner and add extra things

Camera::Camera(
    Window& window,
    const glm::vec3 &position,
    float yaw,
    float pitch,
    float roll,
    float fov,
    float nearClip,
    float farClip,
    ProjectionType type
    )
    : mWindow(window)
    , mPosition(position)
    , mYaw(yaw)
    , mPitch(pitch)
    , mRoll(roll)
    , mFOV(fov)
    , mNearClip(nearClip)
    , mFarClip(farClip)
    , mProjType(type)
    , mProjDirty(true)
    , mViewDirty(true)
{
    mAspect = static_cast<float>(mWindow.getWidth())/static_cast<float>(mWindow.getHeight());
    updateOrientationVectors();
    updateProjectionMatrix();
    updateViewMatrix();
}

void Camera::update(const float deltaTime) {



    const float moveSpeed = 5.0f;

    const Uint8* kb = SDL_GetKeyboardState(nullptr);
    const float velocity = moveSpeed * deltaTime;

    if (kb[SDL_SCANCODE_W]) {
        mPosition += velocity * mForward;
        mViewDirty = true;
    }
    if (kb[SDL_SCANCODE_S]) {
        mPosition += -velocity * mForward;
        mViewDirty = true;
    }
    if (kb[SDL_SCANCODE_A]) {
        mPosition += -velocity * mRight;
        mViewDirty = true;
    }
    if (kb[SDL_SCANCODE_D]) {
        mPosition += velocity * mRight;
        mViewDirty = true;
    }

    int xRel, yRel;
    SDL_GetRelativeMouseState(&xRel, &yRel);

    if (xRel != 0 || yRel != 0) {
        constexpr float sensitivity = 0.1f;
        const float xOffset = -static_cast<float>(xRel) * sensitivity;
        const float yOffset = -static_cast<float>(yRel) * sensitivity;

        mYaw   += xOffset;
        mPitch += yOffset;

        mPitch = std::clamp(mPitch, -89.0f, 89.0f);
        mViewDirty = true;
    }
}



glm::mat4& Camera::getViewMatrix() {
    if (mViewDirty) {
        updateViewMatrix();
    }
    return mViewMatrix;
}

glm::mat4& Camera::getProjectionMatrix() {
    if (mProjDirty) {
        updateProjectionMatrix();
    }
    return mProjMatrix;
}

glm::mat3 Camera::getBasisVectors() {
    if (mViewDirty) {
        updateViewMatrix();
    }
    return glm::mat3(mRight, mUp, mForward);
}



void Camera::updateOrientationVectors() {
    // Create the quaternions
    const glm::quat qRoll = glm::angleAxis(glm::radians(mRoll), glm::vec3(0, 0, -1));
    const glm::quat qPitch = glm::angleAxis(glm::radians(mPitch), glm::vec3(1, 0, 0));
    const glm::quat qYaw = glm::angleAxis(glm::radians(mYaw), glm::vec3(0, 1, 0));

    // Combine the quaternions
    glm::quat orientation = qYaw * qPitch * qRoll;
    orientation = glm::normalize(orientation);

    mForward = orientation * glm::vec3(0, 0, -1);
    mRight = orientation * glm::vec3(1, 0, 0);
    mUp = orientation * glm::vec3(0, 1, 0);
}


void Camera::updateViewMatrix() {
    updateOrientationVectors();

    const glm::vec3 target = mPosition + mForward;
    mViewMatrix = glm::lookAt(mPosition, target, mUp);
    mViewDirty = false;
}


void Camera::updateProjectionMatrix() {
    if (mProjType == ProjectionType::Perspective) {
        mProjMatrix = glm::perspective(
            glm::radians(mFOV),
            mAspect,
            mNearClip,
            mFarClip
        );
    }
    mProjDirty = false;
}
