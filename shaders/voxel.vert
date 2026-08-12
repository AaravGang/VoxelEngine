#version 330 core
layout (location = 0) in vec3 aPos;

// Uniforms are variables passed from the CPU that stay the same for the whole object
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Multiply the position by our 3D transformation matrices
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}