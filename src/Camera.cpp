#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <algorithm>
#include <iostream>
#include <ostream>

Camera::Camera(
    Window& window,
    const glm::vec3& target,
    float distanceToTarget,
    float yaw,
    float pitch,
    float fov,
    float nearClip,
    float farClip,
    ProjectionType type
)   : mWindow(window)
    , mTarget(target)
    , mDistanceToTarget(distanceToTarget)
    , mYaw(yaw)
    , mPitch(pitch)
    , mFOV(fov)
    , mNearClip(nearClip)
    , mFarClip(farClip)
    , mProjType(type)
    , mViewDirty(true)
    , mProjDirty(true)
{
    mAspect = static_cast<float>(mWindow.getWidth()) / mWindow.getHeight();
    updateProjectionMatrix();
    updateViewMatrix();
}

glm::vec3 Camera::getPosition() const {
    return mPosition;
}

glm::vec3 Camera::getTarget() const {
    return mTarget;
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
    if (mViewDirty) updateViewMatrix();
    return glm::mat3(mRight, mUp, mForward);
}

void Camera::update(float deltaTime) {
    int mouseX, mouseY;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
    if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        if (!mFirstMouse) {
            float dx = -(mouseX - mLastMouseX);
            float dy = mouseY - mLastMouseY;

            float sensitivity = 0.3f;
            mYaw   += dx * sensitivity;
            mPitch += dy * sensitivity;
            mPitch = std::clamp(mPitch, -89.0f, 89.0f);
            mViewDirty = true;
        }
        mFirstMouse = false;
    } else {
        mFirstMouse = true;
    }

    if (mouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
        float dx = mouseX - mLastMouseX;
        float dy = mouseY - mLastMouseY;
        float panSpeed = 0.005f * mDistanceToTarget;
        mTarget += -mRight * dx * panSpeed + mUp * dy * panSpeed;
        mViewDirty = true;
    }

    mLastMouseX = mouseX;
    mLastMouseY = mouseY;
}

void Camera::handleEvent(const SDL_Event &e) {
    if (e.type == SDL_MOUSEWHEEL) {
        mDistanceToTarget *= (1.0f - e.wheel.y * 0.1f);
        mDistanceToTarget = std::clamp(mDistanceToTarget, 0.5f, 5000.0f);
        mViewDirty = true;
    }
}

void Camera::updateViewMatrix() {
    float yawRad = glm::radians(mYaw);
    float pitchRad = glm::radians(mPitch);

    glm::vec3 offset;
    offset.x = mDistanceToTarget * cos(pitchRad) * sin(yawRad);
    offset.y = mDistanceToTarget * sin(pitchRad);
    offset.z = mDistanceToTarget * cos(pitchRad) * cos(yawRad);
    mPosition = mTarget + offset;
    mForward = glm::normalize(mTarget - mPosition);
    mRight = glm::normalize(glm::cross(mForward, glm::vec3(0, 1, 0)));
    mUp = glm::normalize(glm::cross(mRight, mForward));

    mViewMatrix = glm::lookAt(mPosition, mTarget, mUp);
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
