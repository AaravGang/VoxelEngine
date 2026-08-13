#version 330 core
in vec3 vertexColor; 
in vec2 TexCoord; // Keeping this here so C++ doesn't complain, ready for real textures later!

out vec4 FragColor;

void main() {
    // Classic Minecraft look: just the raw block color with our pre-calculated C++ shadows!
    FragColor = vec4(vertexColor, 1.0);
}