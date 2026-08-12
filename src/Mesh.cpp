#include "Mesh.h"

Mesh::Mesh()
    : VAO(0)
    , VBO(0)
    , isUploaded(false) {
    // CRITICAL: No OpenGL functions can be called in this constructor!
    // This is executed by background CPU threads, which do not have an OpenGL context.
}

Mesh::~Mesh() {
    // Only delete the buffers if they were actually generated
    if (isUploaded) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}

void Mesh::UploadToGPU() {
    if (!isUploaded) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        isUploaded = true;
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Position Attribute (Location 0, 3 floats, stride is now 6 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color Attribute (Location 1, 3 floats, starts after the first 3 floats)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void Mesh::Draw() {
    if (isUploaded) {
        glBindVertexArray(VAO);

        // We must divide by 6 because every vertex is now 6 floats long (XYZRGB)
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6);
    }
}