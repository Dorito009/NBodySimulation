//
// Created by dorito009 on 6/3/25.
//

#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H
#include "Camera.h"
#include "Window.h"


class Camera;

class EventDispatcher {
public:
    EventDispatcher(Window& window, Camera& camera): m_window(window), m_camera(camera) {}

    bool processEvents();

private:
    Window& m_window;
    Camera& m_camera;

    bool m_closeEvent = false;
};



#endif //EVENTDISPATCHER_H
