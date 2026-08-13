#include "Mesh.h"

Mesh::Mesh()
    : VAO(0)
    , VBO(0)
    , isUploaded(false) { }

Mesh::~Mesh() {
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
}

void Mesh::UploadToGPU() {
    if (vertices.empty()) {
        isUploaded = true;
        return;
    }

    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Location 0: XYZ (3 floats) - Stride is now 8!
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Location 1: RGB (3 floats)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Location 2: UV (2 floats) - Starts after the first 6 floats
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    isUploaded = true;
}

void Mesh::Draw() {
    if (isUploaded && !vertices.empty()) {
        glBindVertexArray(VAO);
        // Divide by 8 because our vertex is now 8 floats long!
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 8);
    }
}