#version 330 core
in vec3 vertexColor; // Received from the vertex shader
out vec4 FragColor;

void main() {
    // Apply the specific block color
    FragColor = vec4(vertexColor, 1.0f);
}