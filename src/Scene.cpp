#include "Scene.hpp"
#include <memory>

Scene::Scene() {

    // vertices must be on the heap, otherwise it would be deleted and then the VAO would not work
    std::vector<float> vertices = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
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
    
    m_shapes.push_back(std::make_unique<Shape>(vertices, indices)); 
}

Scene::~Scene() {
    
}

Shape* Scene::get_shape(size_t idx) const {
    return m_shapes[idx].get();
}