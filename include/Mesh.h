#pragma once
#include "MemoryArena.h"
#include <glad/glad.h>
#include <memory>

class Mesh {
public:
    unsigned int VAO, VBO;
    bool isUploaded;
    size_t vertexCount;

    std::unique_ptr<MeshBuffer> buffer;

    Mesh();
    ~Mesh();

    void UploadToGPU();
    void Draw();
};