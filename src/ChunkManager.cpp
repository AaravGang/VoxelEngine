#include "ChunkManager.h"
#include "Frustum.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

ChunkManager::ChunkManager(ThreadPool& pool)
    : threadPool(pool) { }

void ChunkManager::Update(glm::vec3 cameraPosition) {
    // Process Finished Background Tasks (Handoff)
    {
        std::lock_guard<std::mutex> lock(handoffMutex);
        while (!handoffQueue.empty()) {
            auto item = handoffQueue.front();
            handoffQueue.pop();

            std::pair<int, int> coords = item.first;
            std::shared_ptr<Chunk> finishedChunk = item.second;

            // Upload to GPU on the main thread!
            finishedChunk->mesh.UploadToGPU();

            // ADD THIS DEBUG LINE:
            // std::cout << "Uploaded Chunk at (" << coords.first << ", " << coords.second << ")
            // with "
            //           << finishedChunk->mesh.vertices.size() << " floats." << std::endl;

            // Move into active rendering pool and remove from generating tracker
            activeChunks[coords] = finishedChunk;
            generatingChunks.erase(coords);
        }
    }

    // Calculate which chunk the camera is currently inside
    int currentChunkX = static_cast<int>(cameraPosition.x) / Chunk::CHUNK_SIZE;
    int currentChunkZ = static_cast<int>(cameraPosition.z) / Chunk::CHUNK_SIZE;

    // Account for negative coordinate offsets in C++ integer division
    if (cameraPosition.x < 0)
        currentChunkX -= 1;
    if (cameraPosition.z < 0)
        currentChunkZ -= 1;

    // Scan the render distance around the camera
    for (int x = currentChunkX - renderDistance; x <= currentChunkX + renderDistance; ++x) {
        for (int z = currentChunkZ - renderDistance; z <= currentChunkZ + renderDistance; ++z) {
            std::pair<int, int> chunkCoord = { x, z };

            // If the chunk doesn't exist AND isn't already being generated, spawn a thread task
            if (activeChunks.find(chunkCoord) == activeChunks.end()
                && generatingChunks.find(chunkCoord) == generatingChunks.end()) {

                // Mark as generating
                generatingChunks[chunkCoord] = true;

                // Dispatch to the background Thread Pool
                threadPool.EnqueueTask([this, x, z]() {
                    // Create a new chunk in isolated memory
                    auto newChunk = std::make_shared<Chunk>();

                    // Run the heavy math
                    this->generator.Generate(*newChunk, x, z);
                    newChunk->GenerateMesh();

                    // Safely pass the finished chunk back to the main thread queue
                    std::lock_guard<std::mutex> lock(this->handoffMutex);
                    this->handoffQueue.push({ { x, z }, newChunk });
                });
            }
        }
    }

    // Unload chunks that are outside the render distance
    for (auto it = activeChunks.begin(); it != activeChunks.end();) {
        int cx = it->first.first;
        int cz = it->first.second;

        // If the chunk is further away than our render distance, destroy it
        if (std::abs(cx - currentChunkX) > renderDistance
            || std::abs(cz - currentChunkZ) > renderDistance) {
            // .erase() removes it from the map, dropping the shared_ptr ref count to 0,
            // which automatically deletes the Chunk and its Mesh from RAM and VRAM!
            it = activeChunks.erase(it);
        } else {
            ++it;
        }
    }
}

void ChunkManager::Render(Shader& shader, const glm::mat4& viewProj) {
    // 1. Extract the frustum planes for this exact frame
    Frustum frustum;
    frustum.ExtractFromMatrix(viewProj);

    int chunksTotal = 0;
    int chunksDrawn = 0;

    // 2. Iterate through your chunks map/vector
    // (Note: Adjust 'coord' or 'chunkX/Z' depending on how you structured your map key)
    for (auto& [coord, chunk] : activeChunks) {
        chunksTotal++;

        // Assuming your map key holds the world X and Z coordinates of the chunk
        int chunkX = coord.first; // Change to coord.x if using glm::ivec2
        int chunkZ = coord.second; // Change to coord.y if using glm::ivec2

        glm::vec3 minBounds(chunkX * Chunk::CHUNK_SIZE, 0, chunkZ * Chunk::CHUNK_SIZE);
        glm::vec3 maxBounds(chunkX * Chunk::CHUNK_SIZE + Chunk::CHUNK_SIZE, Chunk::CHUNK_SIZE,
                            chunkZ * Chunk::CHUNK_SIZE + Chunk::CHUNK_SIZE);

        if (frustum.IsBoxVisible(minBounds, maxBounds)) {

            // 2. THE FIX: Translate the local 0-64 geometry to its actual world position!
            glm::mat4 model = glm::translate(glm::mat4(1.0f), minBounds);

            unsigned int modelLoc = glGetUniformLocation(shader.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            // 3. Ensure the mesh is uploaded (from background threads)
            if (!chunk->mesh.isUploaded) {
                chunk->mesh.UploadToGPU();
            }

            chunk->mesh.Draw();
            chunksDrawn++;
        }
    }

    // Uncomment this to watch the optimization in real-time in your terminal!
    // std::cout << "Frustum Culling: Rendering " << chunksDrawn << " / " << chunksTotal << "chunks\n";
}