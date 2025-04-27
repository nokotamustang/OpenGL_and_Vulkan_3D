#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <unordered_map>

#include "model.hpp"

using std::shared_ptr;
using std::unique_ptr;
using std::unordered_map;

struct TransformComponent {
    glm::vec3 translation{};
    glm::vec3 scale{1.f, 1.f, 1.f};
    glm::vec3 rotation{};
    glm::mat4 mat4();
    glm::mat3 normalMatrix();
};

struct PointLightComponent {
    float lightIntensity = 1.0f;
};

class LveGameObject {
   public:
    using id_t = unsigned int;
    using Map = unordered_map<id_t, LveGameObject>;

    static LveGameObject createGameObject() {
        static id_t currentId = 0;
        return LveGameObject{currentId++};
    }

    static LveGameObject makePointLight(
        float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

    LveGameObject(const LveGameObject &) = delete;
    LveGameObject &operator=(const LveGameObject &) = delete;
    LveGameObject(LveGameObject &&) = default;
    LveGameObject &operator=(LveGameObject &&) = default;

    id_t getId() { return id; }

    glm::vec3 color{};
    TransformComponent transform{};

    // Optional pointer components
    shared_ptr<LveModel> model{};
    unique_ptr<PointLightComponent> pointLight = nullptr;

   private:
    LveGameObject(id_t objId) : id{objId} {}

    id_t id;
};
