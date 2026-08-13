#include "GPUCompute.h"
#include "Chunk.h" // For CHUNK_SIZE
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>

GPUCompute::GPUCompute()
    : computeShader("../shaders/noise.vert", "../shaders/noise.frag") {
    // 1. Setup the invisible Framebuffer Object (FBO)
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // 2. Setup a 32-bit FLOAT texture. (Standard textures max out at 1.0, we need real numbers!)
    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);
    // Change GL_R32F to GL_RGBA32F, and GL_RED to GL_RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Chunk::CHUNK_SIZE, Chunk::CHUNK_SIZE, 0, GL_RGBA,
                 GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Attach texture to FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureID, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3. Setup the 2D Screen Quad to give the Fragment Shader a canvas to draw on
    float quadVertices[]
        = { // Positions
            -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f
          };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
}

GPUCompute::~GPUCompute() {
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &TextureID);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}

std::vector<float> GPUCompute::GenerateHeightMap(int chunkWorldX, int chunkWorldZ) {
    // 1. SAVE THE CURRENT VIEWPORT (Your actual window size)
    GLint last_viewport[4];
    glGetIntegerv(GL_VIEWPORT, last_viewport);

    // 2. Bind our invisible FBO and match the viewport to the chunk size
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, Chunk::CHUNK_SIZE, Chunk::CHUNK_SIZE);

    // 3. Execute the shader
    computeShader.Use();
    unsigned int offsetLoc = glGetUniformLocation(computeShader.ID, "chunkOffset");
    glUniform2f(offsetLoc, static_cast<float>(chunkWorldX * Chunk::CHUNK_SIZE),
                static_cast<float>(chunkWorldZ * Chunk::CHUNK_SIZE));

    // 4. Draw the quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Multiply by 4 because there are 4 floats (R, G, B, A) per pixel!
    std::vector<float> heightMap(Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE * 4);

    // Change GL_RED to GL_RGBA
    glReadPixels(0, 0, Chunk::CHUNK_SIZE, Chunk::CHUNK_SIZE, GL_RGBA, GL_FLOAT, heightMap.data());
    
    // 6. Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 7. RESTORE THE ORIGINAL VIEWPORT SO THE GAME RENDERS NORMALLY!
    glViewport(last_viewport[0], last_viewport[1], last_viewport[2], last_viewport[3]);

    return heightMap;
}