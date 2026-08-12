#include "ChunkManager.h"
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
            // std::cout << "Uploaded Chunk at (" << coords.first << ", " << coords.second << ") with "
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

void ChunkManager::Render(Shader& shader) {
    // Iterate through all active chunks and draw them
    for (auto& pair : activeChunks) {
        int x = pair.first.first;
        int z = pair.first.second;

        // Calculate the physical world offset for this specific chunk
        glm::mat4 model = glm::mat4(1.0f);
        model
            = glm::translate(model, glm::vec3(x * Chunk::CHUNK_SIZE, 0.0f, z * Chunk::CHUNK_SIZE));

        unsigned int modelLoc = glGetUniformLocation(shader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Draw the chunk's internal mesh
        pair.second->mesh.Draw();
    }
}