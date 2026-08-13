#version 330 core
in vec3 vertexColor; 
in vec2 TexCoord;

out vec4 FragColor;

void main() {
    // fract() isolates the decimal. It loops from 0.0 to 1.0 for every single block unit.
    vec2 grid = fract(TexCoord);
    
    // Define the thickness of our fake geometry borders (3% of the block)
    float thickness = 0.03;
    
    // If the pixel is near the edge of a 1x1 block, darken the color!
    if (grid.x < thickness || grid.x > 1.0 - thickness || 
        grid.y < thickness || grid.y > 1.0 - thickness) {
        
        // Multiply by 0.6 to create a shadow outline
        FragColor = vec4(vertexColor * 0.6, 1.0); 
    } else {
        // Otherwise, render the normal block color
        FragColor = vec4(vertexColor, 1.0);
    }
}