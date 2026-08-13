#include "FPSCounter.h"

FPSCounter::FPSCounter() {
    lastTime = glfwGetTime();
    nbFrames = 0;
}

void FPSCounter::Update(GLFWwindow* window) {
    double currentTime = glfwGetTime();
    nbFrames++;

    // If exactly 1 second has passed since the last update
    if (currentTime - lastTime >= 1.0) {

        // Update the window title with the new FPS
        std::string title = "Voxel Engine - FPS: " + std::to_string(nbFrames);
        glfwSetWindowTitle(window, title.c_str());

        // Reset the frame counter and advance the timer
        nbFrames = 0;
        lastTime += 1.0;
    }
}