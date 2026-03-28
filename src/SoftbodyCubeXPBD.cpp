#include "SoftbodyCubeXPBD.hpp"
#include <cstring>
#include <iostream>
#include <Eigen/Geometry>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


SoftbodyCubeXPBD::SoftbodyCubeXPBD(glm::vec3 center, glm::vec3 angles, double size)
: m_center(center), m_angles(angles), m_size(size) {
    
    reset_cube();

}

SoftbodyCubeXPBD::~SoftbodyCubeXPBD() {
    m_vertices.clear();
    m_indices.clear();
    m_vertices.shrink_to_fit();
    m_indices.shrink_to_fit();
}

void SoftbodyCubeXPBD::update_vbo() {
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

GLuint SoftbodyCubeXPBD::get_vao() const {
    return m_vao;   
};

void SoftbodyCubeXPBD::simulate(double dt) {

    double prev_time = glfwGetTime();
    double inverse_stiffness_tilde = m_inverse_stiffness / (dt * dt);

    Eigen::VectorXd old_positions = m_positions;
    m_positions = old_positions + dt * m_velocities + dt * dt * m_gravity; // Prediction

    m_lambda = Eigen::VectorXd::Zero(m_num_constraints);

    unsigned first, second, third;
    double length;
    Eigen::Vector3d first_pos, second_pos, third_pos;

    for(int k = 0; k < m_solver_iterations; k++) {
        // Soft constraints
        if(m_distance_constraint_on) {
            //std::cout << "Fulfilling distance constraint" << std::endl;
            // Distance constraints (simple linear spring constraint |xi - xj| - l_ij)
            for(int j = 0; j < m_edges.size(); j++) {
                // Each edge represents one constraint C_j between two particles
                auto& edge = m_edges[j];
                first = std::get<0>(edge);
                second = std::get<1>(edge);
                length = std::get<2>(edge);

                Eigen::Vector3d position_first = m_positions.segment(first * 3, 3);

                Eigen::Vector3d position_second = m_positions.segment(second * 3, 3);

                double Cj = (position_first - position_second).norm() - length;
                Eigen::Vector3d dCj = (position_first - position_second) / (position_first - position_second).norm();

                // TODO: Other point masses except 1
                double delta_lambda_j = (- Cj - inverse_stiffness_tilde * m_lambda[j]) / (1 + 1 + inverse_stiffness_tilde); 
                m_positions.segment(first * 3, 3) += 1 * dCj * delta_lambda_j;
                m_positions.segment(second * 3, 3) -= 1 * dCj * delta_lambda_j;
                m_lambda[j] += delta_lambda_j;
            }
        }
        // Volume constraint
        if(m_volume_constraint_on) {
            //std::cout << "Fulfilling volume constraint" << std::endl;
            double current_volume = calculate_volume();
            double Cj = current_volume - m_goal_volume;
            if (Cj != 0.0) {

                Eigen::MatrixXd dCj = Eigen::MatrixXd(1, m_num_elements);
                
                // Calculate dC/dx
                for(auto& triangle: m_triangles) {
                    first = std::get<0>(triangle);
                    second = std::get<1>(triangle);
                    third = std::get<2>(triangle);

                    first_pos = m_positions.segment(3 * first, 3);
                    second_pos = m_positions.segment(3 * second, 3);
                    third_pos = m_positions.segment(3 * third, 3);

                    dCj.block<1, 3>(0, 3 * first) += second_pos.cross(third_pos) / 6.0;
                    dCj.block<1, 3>(0, 3 * second) += third_pos.cross(first_pos) / 6.0;
                    dCj.block<1, 3>(0, 3 * third) += first_pos.cross(second_pos) / 6.0;

                }

                // Compute delta lambda denominator
                double delta_lambda_denom = (dCj * m_inverse_mass * dCj.transpose()).eval().value() + inverse_stiffness_tilde;
                //double delta_lambda_denom = dCj.dot(m_inverse_mass * dCj) + inverse_stiffness_tilde;

                if (delta_lambda_denom != 0.0) {
                    // The first m_edges.size() elements of lambda correspond to the distance constraints
                    // The element after those, which is indexed at m_edges.size(), corresponds to the volume constraint.
                    double delta_lambda = (-Cj - inverse_stiffness_tilde * m_lambda(m_edges.size())) / delta_lambda_denom; 
                    m_positions += m_inverse_mass * dCj.transpose() * delta_lambda;
                }
            }
        }

        // Hard constraints (Ground)
        for(int i = 1; i < m_num_elements; i += 3) {
            // Check every y-coordinate and set it to the ground.
            // The velocity is set below.
            if (m_positions[i] < 0.0) {
                m_positions[i] = 0.0;
            }
        }
    }

    m_velocities = (m_positions - old_positions) / dt;

    update_vbo();

    std::cout << "XPBD simulation step time: " << glfwGetTime() - prev_time << std::endl;
    
}

size_t SoftbodyCubeXPBD::get_index(size_t i, size_t j, size_t k) {
    return i + m_grid_dim * j + m_grid_dim * m_grid_dim * k;
}

unsigned SoftbodyCubeXPBD::get_grid_dim() {
    return m_grid_dim;
}

void SoftbodyCubeXPBD::set_inverse_stiffness(float inv_stiff) {
    m_inverse_stiffness = inv_stiff;
}

void SoftbodyCubeXPBD::set_solver_iterations(unsigned iters) {
    m_solver_iterations = iters;
}

void SoftbodyCubeXPBD::set_distance_constraint_on(bool val) {
    m_distance_constraint_on = val;
}

void SoftbodyCubeXPBD::set_volume_constraint_on(bool val) {
    m_volume_constraint_on = val;
}

double SoftbodyCubeXPBD::calculate_volume() {
    double volume = 0;
    unsigned first, second, third;
    double triangle_volume;
    Eigen::Vector3d first_pos, second_pos, third_pos;

    for(auto& triangle: m_triangles) {
        first = std::get<0>(triangle);
        second = std::get<1>(triangle);
        third = std::get<2>(triangle);

        first_pos = m_positions.segment(3 * first, 3);
        second_pos = m_positions.segment(3 * second, 3);
        third_pos = m_positions.segment(3 * third, 3);

        triangle_volume = first_pos.cross(second_pos).dot(third_pos) / 6.0;

        volume += triangle_volume;
    }

    return volume;
}

void SoftbodyCubeXPBD::reset_cube() {
    // SIMULATION

    Eigen::Vector3d e_center;
    e_center << m_center.x, m_center.y, m_center.z;

    Eigen::AngleAxisd roll_angle(m_angles.x, Eigen::Vector3d::UnitX());
    Eigen::AngleAxisd pitch_angle(m_angles.y, Eigen::Vector3d::UnitY());
    Eigen::AngleAxisd yaw_angle(m_angles.z, Eigen::Vector3d::UnitZ());

    Eigen::Matrix3d e_rotation = (yaw_angle * pitch_angle * roll_angle).matrix();

    m_num_elements = 3 * 8;
    // Construct m_positions first
    m_positions = Eigen::VectorXd::Zero(m_num_elements);
    double step = m_size / (m_grid_dim-1);
    for (int k = 0; k < m_grid_dim; ++k) {
        for (int j = 0; j < m_grid_dim; ++j) {
            for (int i = 0; i < m_grid_dim; ++i) {
                double x = -m_size/2.0 + i * step;
                double y = -m_size/2.0 + j * step;
                double z = -m_size/2.0 + k * step;

                std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;

                m_positions.segment(3 * get_index(i, j, k), 3) = e_center + e_rotation * Eigen::Vector3d(x, y, z);
            }
        }
    }


    m_velocities = Eigen::VectorXd::Zero(m_num_elements);

    // All particles are set to a mass of 1.
    // TODO: Maybe make this changable in the future
    m_inverse_mass = Eigen::SparseMatrix<double>(m_num_elements, m_num_elements);
    m_inverse_mass.setIdentity();

    m_gravity = Eigen::VectorXd::Zero(m_num_elements);
    for (size_t i = 1; i < m_num_elements; i += 3) {
        m_gravity(i) = -9.81;
    }

    // Construct edges. 
    Eigen::Vector3d difference;
    double length;
    int first, second;
    for (int k = 0; k < m_grid_dim; ++k) {
        for (int j = 0; j < m_grid_dim; ++j) {
            for (int i = 0; i < m_grid_dim; ++i) {
                // Parallel Edges
                if (i + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i+1, j, k);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_edges.push_back({first, second, length});
                }
                if (j + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i, j+1, k);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_edges.push_back({first, second, length});
                }
                if (k + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i, j, k+1);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_edges.push_back({first, second, length});
                }

                // Diagonals
                if (i + 1 < m_grid_dim && j + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i+1, j+1, k);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_edges.push_back({first, second, length});
                }
                if (j + 1 < m_grid_dim && k + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i, j+1, k+1);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_edges.push_back({first, second, length});
                }
                if (i + 1 < m_grid_dim && k + 1 < m_grid_dim) {
                    first = get_index(i, j, k);
                    second = get_index(i+1, j, k+1);
                    length = (m_positions.segment(3 * second, 3) - m_positions.segment(3 * first, 3)).norm();
                    if (first < second)
                        m_edges.push_back({first, second, length});
                }
            }
        }
    }

    // triangle winding order must be consistent for volume to be calculated correctly
    m_triangles = {
        {0, 3, 1},
        {0, 2, 3},

        {7, 3, 2},
        {6, 7, 2},

        {5, 1, 7},
        {1, 3, 7},

        {4, 5, 7},
        {4, 7, 6},

        {0, 6, 2},
        {0, 4, 6},

        {0, 1, 5},
        {0, 5, 4}
    };

    m_num_constraints = m_edges.size() + 1; // num_edges distance constraints + 1 volume constraint
    m_lambda = Eigen::VectorXd::Zero(m_num_constraints);

    m_goal_volume = calculate_volume();
    std::cout << "Volume: " << m_goal_volume << std::endl;

    // RENDERING

    // Use only m_vertices for rendering points, use m_indices additionally to render edges
    m_vertices.clear();
    m_indices.clear();
    m_vertices.reserve(m_num_elements);
    m_indices.reserve(2 * m_edges.size());

    for (auto val: m_positions) {
        m_vertices.push_back(val);
    }

    for (auto edge: m_edges) {
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(int), m_indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0); // TODO: This will need to be generalized
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // Unbind VAO again    
}