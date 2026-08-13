#include "Mesh.h"

Mesh::Mesh()
    : VAO(0)
    , VBO(0)
    , isUploaded(false)
    , vertexCount(0)
    , buffer(nullptr) { }

Mesh::~Mesh() {
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);

    if (buffer) {
        GlobalMeshArena.Free(std::move(buffer));
    }
}

void Mesh::UploadToGPU() {
    if (!buffer || buffer->count == 0) {
        isUploaded = true;
        vertexCount = 0;
        return;
    }

    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, buffer->count * sizeof(float), buffer->data.data(),
                 GL_STATIC_DRAW);

    // Stride is 8! (XYZ, RGB, UV)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    vertexCount = buffer->count / 8; // Divide by 8 to get vertex count
    isUploaded = true;

    // Return memory to Arena instantly!
    GlobalMeshArena.Free(std::move(buffer));
}

void Mesh::Draw() {
    if (isUploaded && vertexCount > 0) {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
}