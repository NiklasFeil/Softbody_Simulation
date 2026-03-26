#include "Scene.hpp"
#include <memory>
#include <glm/ext.hpp>
#include "ObjLoader.hpp"
#include <iostream>

Scene::Scene(unsigned sim, const char* obj)
: m_current_simulation(sim), m_current_object(obj) {

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

    m_solids.push_back(std::make_unique<Solid>(vertices, indices, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.0f, 0.5f, 5.0f))); 

    m_sb_cube_ms = std::make_unique<SoftbodyCubeMassSpring>(3, glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 45.0f), 2);
    m_sb_cube_xpbd = std::make_unique<SoftbodyXPBD>(ObjLoader::load("../models/cube.obj"), glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 45.0f), glm::vec3(1.0, 1.0, 1.0));
    m_sb_sphere_xpbd = std::make_unique<SoftbodyXPBD>(ObjLoader::load("../models/sphere.obj"), glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 45.0f), glm::vec3(1.0, 1.0, 1.0));
}

Scene::~Scene() {
    
}

Solid* Scene::get_solid(size_t idx) const {
    return m_solids[idx].get();
}

SoftbodyCubeMassSpring* Scene::get_sb_cube_ms() const {
    return m_sb_cube_ms.get();
}

SoftbodyXPBD* Scene::get_sb_obj_xpbd(std::string obj) const {
    if (obj == "sphere")
        return m_sb_sphere_xpbd.get();
    else if (obj == "cube")
        return m_sb_cube_xpbd.get();
    else
        throw std::invalid_argument("Given object does not exist");
}

unsigned Scene::get_current_sim() const {
    return m_current_simulation;
}

std::string Scene::get_current_object() const {
    return m_current_object;
}

void Scene::set_current_object(std::string obj) {
    m_current_object = obj;
}

void Scene::set_current_sim(unsigned sim) {
    m_current_simulation = sim;
}

void Scene::reset() {
    m_sb_cube_ms->reset_cube();
    m_sb_cube_xpbd->reset_object();
    m_sb_sphere_xpbd->reset_object();
}