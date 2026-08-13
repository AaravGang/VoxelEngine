#pragma once
#include <glad/glad.h>
#include <vector>

class Mesh {
public:
    unsigned int VAO, VBO;
    bool isUploaded;
    
    // We are back to the standard vector!
    std::vector<float> vertices; 

    Mesh();
    ~Mesh();

    void UploadToGPU();
    void Draw();
};