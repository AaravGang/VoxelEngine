#pragma once
#include <glad/glad.h>
#include <vector>

class Mesh {
public:
    std::vector<float> vertices;
    unsigned int VAO, VBO;
    bool isUploaded; // Tracks if the GPU buffers exist

    Mesh();
    ~Mesh();

    void UploadToGPU();
    void Draw();
};