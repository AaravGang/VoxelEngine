#version 330 core
in vec2 WorldPos;
out vec4 FragColor; 

// --- GLSL 2D Simplex Noise (Ashima Arts) ---
vec3 permute(vec3 x) { return mod(((x*34.0)+1.0)*x, 289.0); }
float snoise(vec2 v){
  const vec4 C = vec4(0.211324865405187, 0.366025403784439,
           -0.577350269189626, 0.024390243902439);
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);
  vec2 i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
  i = mod(i, 289.0);
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
  + i.x + vec3(0.0, i1.x, 1.0 ));
  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
    dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}
// -------------------------------------------

// --- CLASSIC FBM (Smooth, Natural Hills) ---
float fbm(vec2 pos) {
    float total = 0.0;
    float amplitude = 1.0;
    float frequency = 1.0;
    float maxValue = 0.0;
    
    // 4 octaves is the standard for classic procedural terrain
    for(int i = 0; i < 4; i++) { 
        total += snoise(pos * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    return total / maxValue;
}
void main() {
    // 1. Calculate the Biome FIRST
    float biomeNoise = snoise((WorldPos + vec2(1234.5, 5678.9)) / 400.0); 
    
    // Mapped from 0.0 to 4.99 (0=Desert, 1=Plains, 2=Forest, 3=Mushroom, 4=Winter)
    float mappedBiome = ((biomeNoise + 1.0) * 0.5) * 4.99; 
    float activeBiome = floor(mappedBiome);
    
    // 2. Base Elevation Noise
    float rawNoise = fbm(WorldPos / 250.0);
    float normalized = (rawNoise + 1.0) * 0.5;
    
    // 3. Math A: Flat / Hilly Terrain (Max height ~35)
    float flatHeight = pow(normalized, 1.2) * 20.0 + 15.0; 
    
    // 4. Math B: Towering Mountains (Max height ~90, steeper peaks)
    float mountainHeight = pow(normalized, 1.6) * 75.0 + 15.0; 
    
    // 5. THE MAGIC: Smoothly blend the two terrains based on the biome map!
    // smoothstep(2.0, 4.0, mappedBiome) means:
    // Biomes 0, 1, 2 = 100% Flat Math
    // Biome 3 (Mushroom) = 50% Flat, 50% Mountain (Foothills)
    // Biome 4 (Winter) = 100% Mountain Math
    float mountainMask = smoothstep(2.0, 4.0, mappedBiome); 
    
    // Mix linearly interpolates between the two heights using the mask
    float terrainHeight = mix(flatHeight, mountainHeight, mountainMask);
    
    // 6. Snowline (Organic block variation)
    float snowNoise = (snoise(WorldPos / 20.0) * 6.0) + (snoise(WorldPos / 4.0) * 3.0);
    float snowLine = 40.0 + snowNoise; // Raised slightly to match the new peaks

    FragColor = vec4(terrainHeight, activeBiome, snowLine, 1.0);
}