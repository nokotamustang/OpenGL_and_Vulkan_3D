#pragma once

#include <functional>

#include "buffer.hpp"
#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <memory>
#include <vector>

using std::size_t;
using std::unique_ptr;
using std::vector;

template <typename T, typename... Rest>
void combine_hash(size_t &seed, const T &v, const Rest &...rest) {
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (combine_hash(seed, rest), ...);
};

class Model {
   public:
    struct Vertex {
        glm::vec3 position{};
        glm::vec3 color{};
        glm::vec3 normal{};
        glm::vec2 uv{};

        static vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

        bool operator==(const Vertex &other) const {
            return position == other.position && color == other.color && normal == other.normal &&
                   uv == other.uv;
        }
    };

    struct Builder {
        vector<Vertex> vertices{};
        vector<uint32_t> indices{};

        void loadModel(const string &filepath);
    };

    Model(Device &device, const Model::Builder &builder);
    ~Model();

    Model(const Model &) = delete;
    Model &operator=(const Model &) = delete;

    static unique_ptr<Model> createModelFromFile(
        Device &device, const string &filepath);

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

   private:
    void createVertexBuffers(const vector<Vertex> &vertices);
    void createIndexBuffers(const vector<uint32_t> &indices);

    Device &device;

    unique_ptr<Buffer> vertexBuffer;
    uint32_t vertexCount;

    bool hasIndexBuffer = false;
    unique_ptr<Buffer> indexBuffer;
    uint32_t indexCount;
};
