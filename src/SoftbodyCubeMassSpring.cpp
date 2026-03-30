#include "SoftbodyCubeMassSpring.hpp"
#include <cstring>
#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

SoftbodyCubeMassSpring::SoftbodyCubeMassSpring(unsigned grid_dim, glm::vec3 center, glm::vec3 angles, double size)
: m_grid_dim(grid_dim), m_center(center), m_angles(angles), m_size(size) {
    
    reset_cube();
    
}

SoftbodyCubeMassSpring::~SoftbodyCubeMassSpring() {
    m_vertices.clear();
    m_indices.clear();
    m_vertices.shrink_to_fit();
    m_indices.shrink_to_fit();
}

void SoftbodyCubeMassSpring::update_vbo() {
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

GLuint SoftbodyCubeMassSpring::get_vao() const {
    return m_vao;   
};

void SoftbodyCubeMassSpring::simulate(double dt) {
    
    double prev_time = glfwGetTime();

    // Construct force vector
    m_force.setZero();

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

        double extension = (position_difference_norm - length);

        double spring_force = m_spring_constant_linear * extension + m_spring_constant_cubic * extension * extension * extension;
        double dampening_force = m_dampening_constant * velocity_difference.dot(direction_ij);

        Eigen::Vector3d force_ij = (spring_force + dampening_force) * direction_ij;

        m_force.segment(i * 3, 3) +=  force_ij;
        m_force.segment(j * 3, 3) += -force_ij;
    }

    // Penality force for particles in the floor ( y < 0 )
    for (size_t i = 1; i < m_positions.size(); i += 3) {
        double y = m_positions(i);
        double vy = m_velocities(i);
        if (y < 0.0) {

            // Apply force upwards
            m_force(i) += - m_penalty_constant * y - m_penalty_dampening_constant * vy;

            // Hard constraint
            m_positions[i] = 0.0;

            if (vy < 0.0) {
                m_velocities[i] = 0.0; 
                // Instead of applying force, one could also reflect velocity
            }

        }
    }

    // Semi-implicit Euler
    std::cout << "m_particle_mass: " << m_particle_mass << std::endl;
    Eigen::VectorXd acc = (1.0 / m_particle_mass) *  m_force + m_gravity_multiplier * m_gravity;
    Eigen::VectorXd dv = dt * acc;
    m_velocities += dv;
    Eigen::VectorXd dx = dt * m_velocities;
    m_positions += dx;

    update_vbo();

    std::cout << "Mass spring simulation step time: " << glfwGetTime() - prev_time << std::endl;
}

size_t SoftbodyCubeMassSpring::get_index(size_t i, size_t j, size_t k) {
    return i + m_grid_dim * j + m_grid_dim * m_grid_dim * k;
}

unsigned SoftbodyCubeMassSpring::get_grid_dim() {
    return m_grid_dim;
}

void SoftbodyCubeMassSpring::reset_cube() {

    Eigen::Vector3d e_center;
    e_center << m_center.x, m_center.y, m_center.z;

    Eigen::AngleAxisd roll_angle(m_angles.x, Eigen::Vector3d::UnitX());
    Eigen::AngleAxisd pitch_angle(m_angles.y, Eigen::Vector3d::UnitY());
    Eigen::AngleAxisd yaw_angle(m_angles.z, Eigen::Vector3d::UnitZ());

    Eigen::Matrix3d e_rotation = (yaw_angle * pitch_angle * roll_angle).matrix();

    m_num_elements = 3 * m_grid_dim * m_grid_dim * m_grid_dim;
    // Construct m_positions first
    m_positions = Eigen::VectorXd::Zero(m_num_elements);
    double step = m_size / (m_grid_dim - 1);
    for (int k = 0; k < m_grid_dim; ++k) {
        for (int j = 0; j < m_grid_dim; ++j) {
            for (int i = 0; i < m_grid_dim; ++i) {
                double x = -m_size/2.0 + i * step;
                double y = -m_size/2.0 + j * step;
                double z = -m_size/2.0 + k * step;

                m_positions.segment(3 * get_index(i, j, k), 3) = e_center + e_rotation * Eigen::Vector3d(x, y, z);
            }
        }
    }

    m_velocities = Eigen::VectorXd::Zero(m_num_elements);

    // Construct springs. 
    m_springs.clear();
    Eigen::Vector3d difference;
    double length;
    int first, second;
    for (int k = 0; k < m_grid_dim; ++k) {
        for (int j = 0; j < m_grid_dim; ++j) {
            for (int i = 0; i < m_grid_dim; ++i) {
                // Structural Springs
                if (i + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i+1, j, k);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});
                    
                }
                if (j + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i, j+1, k);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});
                    
                }
                if (k + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i, j, k+1);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});
                    
                }

                // Shear Springs
                if (i + 1 < m_grid_dim && j + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i+1, j+1, k);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});
                }
                if (j + 1 < m_grid_dim && k + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i, j+1, k+1);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});
                }
                if (i + 1 < m_grid_dim && k + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i+1, j, k+1);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});
                }
                if (i - 1 >= 0 && j + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i-1, j+1, k);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});
                }
                if (j - 1 >= 0 && k + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i, j-1, k+1);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});
                }
                if (i - 1 >= 0 && k + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i-1, j, k+1);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});
                }

                if (i + 1 < m_grid_dim && j + 1 < m_grid_dim && k + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i+1, j+1, k+1);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});
                }
                if (i - 1 >= 0 && j + 1 < m_grid_dim && k + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i-1, j+1, k+1);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});
                }
                if (i + 1 < m_grid_dim && j - 1 >= 0 && k + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i+1, j-1, k+1);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});
                }
                if (i + 1 < m_grid_dim && j + 1 < m_grid_dim && k - 1 >= 0) {
                    first = get_index(i, j, k);
                    second = get_index(i+1, j+1, k-1);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});
                }
                if (i - 1 >= 0 && j - 1 >= 0 && k + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i-1, j-1, k+1);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});   
                }

                // Bend Springs
                if (i + 2 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i+2, j, k);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});
                    
                }
                if (j + 2 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i, j+2, k);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});
                    
                }
                if (k + 2 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i, j, k+2);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_springs.push_back({first, second, length});
                    
                }
            }
        }
    }

    m_gravity = Eigen::VectorXd::Zero(m_num_elements);
    for (size_t i = 1; i < m_num_elements; i += 3) {
        m_gravity(i) = -9.81;
    }

    m_force = Eigen::VectorXd::Zero(m_num_elements);

    // RENDERING

    m_vertices.clear();
    m_indices.clear();
    m_vertices.reserve(m_num_elements);
    m_indices.reserve(m_num_elements);

    for (auto val: m_positions) {
        m_vertices.push_back(val);
    }

    for (auto edge: m_springs) {
        unsigned first = std::get<0>(edge);
        unsigned second = std::get<1>(edge);

        m_indices.push_back(first);
        m_indices.push_back(second);
    }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    std::cout << "m_indices.size(): " << m_indices.size() << std::endl;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(int), m_indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0); // TODO: This will need to be generalized
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // Unbind VAO again

}

const Eigen::VectorXd& SoftbodyCubeMassSpring::get_positions() const {
    return m_positions;
}