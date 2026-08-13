#pragma once
#include <GLFW/glfw3.h>
#include <string>

class FPSCounter {
private:
    double lastTime;
    int nbFrames;

public:
    FPSCounter();

    // Call this exactly once per frame inside your main loop
    void Update(GLFWwindow* window);
};