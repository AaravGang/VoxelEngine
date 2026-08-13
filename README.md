# **VoxelEngine: Dual-Architecture C++/OpenGL Engine**

A high-performance, multithreaded voxel terrain generator built in C++ and OpenGL. This project serves as an architectural study in rendering performance, procedural generation, and CPU vs. GPU bottleneck management.

To prove the efficacy of hardware-specific optimizations, this engine features two completely distinct generation architectures: **Engine A** (CPU-Maximalist) and **Engine B** (GPU-Compute).

## **📊 Performance Benchmarks & Metrics**

The optimization pipeline was highly iterative. Below are the performance metrics recorded at each stage of the engine's development.

### **Iterative Optimization Scaling**

| Optimization Stage | Frame Rate (FPS) | Notes & Observations |
| :---- | :---- | :---- |
| **Naive Meshing** | 80 FPS | Baseline performance drawing every block face. |
| **Greedy Meshing** | 250 FPS | Massive bandwidth reduction; vertices slashed by combining adjacent faces. |
| **Memory Arena** | 250 FPS | Eliminated OS-level new/delete overhead, resulting in **zero frame jitter**. |
| **SIMD (CPU Math)** | 310 FPS | CPU math executes so fast that pushing camera speed to 500 results in instant chunk loading (bottleneck shifted to GPU render speed). |
| **Frustum Culling** | 400 FPS | CPU completely ignores rendering geometry behind the camera. |
| **GPU Compute** | 500+ FPS | Re-architected pipeline to bypass CPU limitations completely. |

### 

### **Architectural Latency Comparison (Per-Chunk)**

Using std::chrono high-resolution profiling, we measured the exact microsecond latency required to generate a single 64x64x64 chunk of terrain.

| Pipeline | Math Generation | Structuring & Meshing | Total Latency |
| :---- | :---- | :---- | :---- |
| **Engine A (CPU SIMD)** | 1,914 µs | 11,316 µs | \~13.2 ms |
| **Engine B (GPU Compute)** | **179 µs** | 19,555 µs | **\~19.7 ms** |

*Note: While Engine B's structuring time is higher (due to the CPU having to read the massive GPU array back over the PCIe bus and executing the spatial hash for trees), the mathematical generation is **over 10x faster**, successfully offloading the heaviest computational burden away from the CPU cores.*

## **🏗️ The Two Architectures**

### **Engine A: CPU-Maximalist** 

Built to push modern CPU scheduling and memory management to its absolute limit.

* **SIMD Intrinsics:** Processes 8 Simplex noise calculations simultaneously per core.  
* **Cache-Line Alignment:** Enforces C++ alignas(64) on chunk arrays to prevent false sharing between Thread Pool workers.  
* **Stride-1 Memory Traversal:** Ensures the hardware prefetcher keeps the L1 cache hot during generation.  
* **Pipeline:** CPU Threads handle Math → Memory Allocation → Greedy Meshing → Main Thread GPU Upload.

### **Engine B: GPU-Compute** 

Built as a counter-architecture to completely bypass CPU lane limits.

* **Legacy GPGPU:** Utilizes an invisible Framebuffer Object (FBO) and a 64x64 Quad to hijack the fragment shader for parallel mathematics.  
* **RGBA Data Packing:** Calculates fractal elevation (R), biome selection (G), and dynamic snowlines (B) simultaneously across thousands of GPU cores.  
* **Topological Blending:** Uses GLSL smoothstep to seamlessly morph flat plains into towering, jagged mountain ranges based on the biome map.  
* **Pipeline:** GPU calculates terrain in parallel → CPU Threads read data, apply deterministic spatial hashing for trees, allocate memory, and generate Greedy Mesh.

## **🧗 Hurdles & Engineering Solutions**

### **1\. The macOS OpenGL 4.1 Limitation**

* **The Hurdle:** Apple officially deprecated OpenGL in 2018, locking macOS at OpenGL 4.1. Compute Shaders (.comp) and SSBOs require OpenGL 4.3+, making standard GPU compute impossible.  
* **The Solution:** Engineered a Legacy GPGPU workaround. Generated a 32-bit Float Texture attached to an FBO and used a Vertex/Fragment shader combo to trick the GPU into running pure mathematics, reading the pixel colors back to the CPU as terrain heights.

### **2\. The OpenGL Context Trap (Multithreading)**

* **The Hurdle:** Attempting to call the GPU Compute pipeline from the background C++ Thread Pool resulted in instant driver crashes, as OpenGL state machines are strictly locked to the Main Thread.  
* **The Solution:** Split the pipeline. The Main Thread fires the compute shader and captures the resulting heightmap vector by value. This pre-computed array is then passed into the background Thread Pool's lambda capture list, allowing background threads to structure the memory without touching the OpenGL context.

### **3\. Apple Clang C++20 Template Bug**

* **The Hurdle:** Upgrading to C++20 for strict cache alignment caused the GLM math library to crash during compilation due to explicit specialization of undeclared template structs (is\_int).  
* **The Solution:** Injected add\_compile\_definitions(GLM\_FORCE\_CXX17) into CMake to force the library down a stable parsing path while preserving C++20 features for the main engine.

### **4\. "Plastic" Procedural Terrain**

* **The Hurdle:** Standard Simplex noise created terrain that looked like smooth, rolling plastic hills, and the snow generated in straight, unnatural flat lines.  
* **The Solution:** Implemented **Fractal Brownian Motion (fBm)** with exponentiation (pow()) to flatten valleys and spike mountains. Added high-frequency dither (WorldPos / 4.0) to the snowline calculation, creating scattered, jagged, and organic frost transitions.

## **🚀 Step-by-Step Build Instructions**

### **Prerequisites**

* CMake (3.12+)  
* C++17 / C++20 compatible compiler (Apple Clang / GCC / MSVC)  
* GLFW3

### **Compiling**

Bash
```  
mkdir build  
cd build  
cmake ..  
make
```

### **Execution**

Bash  
``` ./VoxelEngine
```

*Hold LEFT SHIFT to increase camera speed and observe the multithreaded chunk generation in real-time.*

## **🔮 Future Improvements**

1. **Infinite Vertical Chunking (Cubic Chunks):** Decouple the Y-axis from the standard 64-block limit to allow for infinite deep caves and atmospheric mountains.  
2. **Global Illumination & AO:** Implement Voxel Cone Tracing or simple Corner Ambient Occlusion within the Greedy Meshing algorithm.  
3. **Entity Component System (ECS):** Integrate a lightweight ECS to handle dynamic voxel updates (e.g., falling sand, flowing water) without blocking the primary rendering loop.

**Author:** Aarav Gang

**Contact:** aaravgang02@gmail.com
