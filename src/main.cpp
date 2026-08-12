#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "Chunk.h"
#include "Mesh.h"
#include "Shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Voxel Engine", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Enable Depth Testing so triangles in the back do not draw over triangles in the front
    glEnable(GL_DEPTH_TEST);

    // Optional: Turn on wireframe mode to actually see the individual voxel faces!
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Shader ourShader("../shaders/voxel.vert", "../shaders/voxel.frag");

    // Instantiate our Chunk and Mesh, then run the algorithm
    Chunk myChunk;
    Mesh myMesh;
    myChunk.GenerateMesh(myMesh);
    myMesh.UploadToGPU();

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Clear both color and depth buffers
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.Use();

        // Create 3D Matrices
        glm::mat4 model = glm::mat4(1.0f);

        // Move the camera back 60 units and up 40 units, looking at the center of the chunk
        glm::mat4 view
            = glm::lookAt(glm::vec3(-30.0f, 40.0f, -30.0f), // Camera position
                          glm::vec3(16.0f, 0.0f, 16.0f), // Target position (center of chunk)
                          glm::vec3(0.0f, 1.0f, 0.0f) // Up vector
            );

        // Set perspective (Field of View, Aspect Ratio, Near Clip, Far Clip)
        glm::mat4 projection
            = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 1000.0f);

        // Send matrices to the GPU
        unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
        unsigned int projLoc = glGetUniformLocation(ourShader.ID, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Draw the chunk
        myMesh.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}