#include "Solid.hpp"
#include <glm/gtc/matrix_transform.hpp>

Solid::Solid(std::vector<float> vertices, std::vector<int> indices, glm::vec3 position, glm::vec3 angles, glm::vec3 scale)
    : m_vertices(vertices), m_indices(indices), m_position(position), m_angles(angles), m_scale(scale) {
    
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(int), m_indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0); // TODO: This will need to be generalized
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // Unbind VAO again  

    // Construct model matrix
    // First apply scale, then rotation (RPY XYZ / Euler ZYX), then translation
    m_model = glm::mat4(1.0f);
    m_model = glm::scale(m_model, m_scale);
    m_model = glm::rotate(m_model, m_angles.x, glm::vec3(1.0f, 0.0f, 0.0f));
    m_model = glm::rotate(m_model, m_angles.y, glm::vec3(0.0f, 1.0f, 0.0f));
    m_model = glm::rotate(m_model, m_angles.z, glm::vec3(0.0f, 0.0f, 1.0f));
    m_model = glm::translate(m_model, m_position);
}

Solid::Solid(float* vertices, int* indices) {

    m_vertices.assign(vertices, vertices + (sizeof(vertices) / sizeof(vertices[0])));
    m_indices.assign(indices, indices + (sizeof(indices) / sizeof(indices[0])));

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(int), m_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0); // TODO: This will need to be generalized
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // Unbind VAO again   

}

Solid::~Solid() {
    m_vertices.clear();
    m_indices.clear();
    m_vertices.shrink_to_fit();
    m_indices.shrink_to_fit();
}

GLuint Solid::get_vao() {
    return m_vao;
}


glm::mat4 Solid::get_model() {
    return m_model;
}