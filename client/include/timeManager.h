#pragma once

#include <GLFW/glfw3.h>  // For time management (using GLFW for time)

// The time manager is used to make the velocity of a player independent of their fps.
class CTIMEMANAGER {
public:
    CTIMEMANAGER() : m_fLastTime(0.0f), m_fDeltaTime(0.0f) {}

    // Initialize the timer
    void init() {
        m_fLastTime = glfwGetTime();  // Initialize lastTime to the current time at startup
    }

    // Update deltaTime
    void update() {
        float currentTime = glfwGetTime();  // Get the current time
        m_fDeltaTime = currentTime - m_fDeltaTime; // Calculate time passed since last frame
        m_fLastTime = currentTime;             // Update lastTime for next frame
    }

    // Get deltaTime (time passed since last frame)
    float getDeltaTime() const {
        return m_fDeltaTime;
    }

private:
    float m_fLastTime;   // Time of the last frame
    float m_fDeltaTime;  // Time between current and last frame (in seconds)
};