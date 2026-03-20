#include "Scene.hpp"
#include <memory>
#include <glm/ext.hpp>

Scene::Scene() {

    // vertices must be on the heap, otherwise it would be deleted and then the VAO would not work
    std::vector<float> vertices = {
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
    };

    std::vector<int> indices = {
        1, 0, 3,
        0, 3, 2,

        5, 1, 7, 
        1, 7, 3,

        4, 5, 6,
        5, 6, 7,

        0, 4, 2, 
        4, 2, 6,

        4, 5, 0,
        5, 0, 1,

        6, 7, 2,
        7, 2, 3
    };
    

    m_solids.push_back(std::make_unique<Solid>(vertices, indices, glm::vec3(0.0f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.0f, 0.5f, 5.0f))); 
    m_sb_cube = std::make_unique<SoftbodyCube>(4, glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 45.0f), 2);
}

Scene::~Scene() {
    
}

Solid* Scene::get_solid(size_t idx) const {
    return m_solids[idx].get();
}

SoftbodyCube* Scene::get_sb_cube() const {
    return m_sb_cube.get();
}