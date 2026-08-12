#pragma once
#include <glad/glad.h>
#include <vector>

class Mesh {
public:
    std::vector<float> vertices;
    unsigned int VAO, VBO;

    Mesh();
    ~Mesh();

    // Sends the vector data to the GPU
    void UploadToGPU();

    // Tells the GPU to draw the uploaded data
    void Draw();
};