#include "SoftbodyCube.hpp"
#include <cstring>
#include <iostream>

SoftbodyCube::SoftbodyCube(std::vector<float> vertices, std::vector<int> indices, glm::vec3 center)
: m_vertices(vertices), m_indices(indices), m_center(center) {
    
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(int), m_indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0); // TODO: This will need to be generalized
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // Unbind VAO again

    // Construct m_positions from m_vertices
    for (size_t i = 0; i < m_vertices.size(); i += 1) {
        m_positions(i) = m_vertices[i];
    }

    // Translate positions so set center aligns
    for (size_t i = 0; i < m_positions.size(); i += 3) {
        m_positions(i  ) += m_center.x;
        m_positions(i+1) += m_center.y;
        m_positions(i+2) += m_center.z;
    }

    update_vbo();

    // Construct springs. Every corner is connected to each other.
    Eigen::Vector3d difference;
    double length;
    for(size_t i = 0; i < 8; i++) {
        for (size_t j = i + 1; j < 8; j++) {
            difference = m_positions.segment(i * 3, 3) - m_positions.segment(j * 3, 3);
            length = difference.norm();
            m_springs.emplace_back(i, j, length);
        }
    }

    // All particles are set to a mass of 1.
    // TODO: Maybe make this changable in the future
    m_inverse_mass.setIdentity();

    for (size_t i = 1; i < m_num_elements; i += 3) {
        m_gravity(i) = -9.81;
    }
}

SoftbodyCube::~SoftbodyCube() {
    m_vertices.clear();
    m_indices.clear();
    m_vertices.shrink_to_fit();
    m_indices.shrink_to_fit();
}

void SoftbodyCube::update_vbo() {
    // Construct new m_vertices
    m_vertices.clear();
    for (unsigned i = 0; i < m_num_elements; i++) {
        m_vertices.push_back((float) m_positions(i));
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    
    // Possibility 1: Redefine Buffer Data
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_DYNAMIC_DRAW);

    // Possibility 2: Use Mapping and memcpy
    //void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    //std::memcpy(ptr, m_vertices.data(), m_vertices.size() * sizeof(float));
    //glUnmapBuffer(GL_ARRAY_BUFFER);
}

GLuint SoftbodyCube::get_vao() const {
    return m_vao;   
};

void SoftbodyCube::simulate_mass_spring(double dt) {
    
    // Construct force vector
    Eigen::VectorXd f = Eigen::VectorXd::Zero(m_num_elements);

    // Forces by springs
    for (auto& spring: m_springs) {
        unsigned i = std::get<0>(spring);
        unsigned j = std::get<1>(spring);
        double length = std::get<2>(spring);

        Eigen::Vector3d position_i = m_positions.segment(i * 3, 3);
        Eigen::Vector3d velocity_i = m_velocities.segment(i * 3, 3);

        Eigen::Vector3d position_j = m_positions.segment(j * 3, 3);
        Eigen::Vector3d velocity_j = m_velocities.segment(j * 3, 3);

        // From i to j
        Eigen::Vector3d position_difference = position_j - position_i; 
        double position_difference_norm = position_difference.norm();
        Eigen::Vector3d velocity_difference = velocity_j - velocity_i;

        Eigen::Vector3d direction_ij = position_difference / position_difference_norm;

        double spring_force = m_spring_constant * (position_difference_norm - length);
        double dampening_force = m_dampening_constant * velocity_difference.dot(direction_ij);

        Eigen::Vector3d force_ij = (spring_force + dampening_force) * direction_ij;
        f.segment(i * 3, 3) += force_ij;
        f.segment(j * 3, 3) += -force_ij;
    }

    // Penality force for particles in the floor ( y < 0 )
    for (size_t i = 1; i < m_positions.size(); i += 3) {
        double y = m_positions(i);
        double vy = m_velocities(i);
        if (y < 0.0) {
            f(i) += - m_penalty_constant * y - m_penalty_dampening_constant * vy;
        }
    }

    // Semi-implicit Euler
    Eigen::VectorXd acc = m_inverse_mass * f + m_gravity;
    Eigen::VectorXd dv = dt * acc;
    Eigen::VectorXd dx = dt * dv;

    m_positions += dx;
    m_velocities += dv;

    update_vbo();
}