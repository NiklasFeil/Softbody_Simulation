#include "Scene.hpp"
#include <memory>

Scene::Scene() {

    // Add triangle to scene
    // vertices must be on the heap, otherwise it would be deleted and then the VAO would not work
    std::vector<float> vertices = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f
    };

    std::vector<int> indices = {
        0, 1, 2,
        1, 2, 3
    };
    
    m_shapes.push_back(std::make_unique<Shape>(vertices, indices)); 

    // Add cube to scene
    //std::vector<float> 
}

Scene::~Scene() {
    
}

Shape* Scene::get_shape(size_t idx) const {
    return m_shapes[idx].get();
}