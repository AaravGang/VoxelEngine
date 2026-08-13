#version 330 core
layout (location = 0) in vec2 aPos;

// We will pass the world coordinates of the chunk as a uniform
uniform vec2 chunkOffset;
out vec2 WorldPos;

void main() {
    // The quad goes from -1.0 to 1.0 (covering the whole invisible FBO)
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    
    // Map the -1 to +1 quad coordinates to 0 to 64 chunk coordinates
    vec2 localPos = (aPos + 1.0) * 0.5 * 64.0;
    
    // Send the absolute world position to the fragment shader
    WorldPos = localPos + chunkOffset;
}