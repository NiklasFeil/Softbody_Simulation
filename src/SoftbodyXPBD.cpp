#include "SoftbodyXPBD.hpp"
#include <cstring>
#include <iostream>
#include <Eigen/Geometry>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <random>

SoftbodyXPBD::SoftbodyXPBD(Obj* obj, glm::vec3 center, glm::vec3 angles, glm::vec3 scale)
: m_obj(obj), m_center(center), m_angles(angles), m_scale(scale) {
    
    reset_object();

}

SoftbodyXPBD::~SoftbodyXPBD() {
    m_vertices.clear();
    m_indices.clear();
    m_vertices.shrink_to_fit();
    m_indices.shrink_to_fit();
}

void SoftbodyXPBD::update_vbo() {
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

GLuint SoftbodyXPBD::get_vao() const {
    return m_vao;   
};

void SoftbodyXPBD::simulate(double dt) {

    //double prev_time = glfwGetTime();
    
    //std::cout << "m_particle_mass: " << m_particle_mass << std::endl;

    double inverse_stiffness_tilde = m_inverse_stiffness / (dt * dt);

    Eigen::VectorXd old_positions = m_positions;
    m_positions = old_positions + dt * m_velocities + dt * dt * m_gravity_multiplier * m_gravity; // Prediction
    m_lambda.setZero();

    unsigned first, second, third;
    double length;
    Eigen::Vector3d first_pos, second_pos, third_pos;

    std::random_device rd;
    std::mt19937 gen {rd()};
    std::shuffle(m_edge_indices.begin(), m_edge_indices.end(), gen);

    for(int k = 0; k < m_solver_iterations; k++) {
        // Soft constraints
        if(m_distance_constraint_on) {
            // Distance constraints (simple linear spring constraint |xi - xj| - l_ij)

            //for(int j = 0; j < m_edges.size(); j++) {
            for(const auto& j : m_edge_indices) {
                // Each edge represents one constraint C_j between two particles
                auto& edge = m_edges[j];
                first = std::get<0>(edge);
                second = std::get<1>(edge);
                length = std::get<2>(edge);

                Eigen::Vector3d position_first = m_positions.segment(first * 3, 3);

                Eigen::Vector3d position_second = m_positions.segment(second * 3, 3);

                double diff_norm = (position_first - position_second).norm();

                if (diff_norm < 1e-7) continue; // Prevent some weird behaviours because of division

                double Cj = diff_norm - length;
                Eigen::Vector3d dCj = (position_first - position_second) / diff_norm;

                double delta_lambda_j = (- Cj - inverse_stiffness_tilde * m_lambda[j]) / (2 * (1 / m_particle_mass) + inverse_stiffness_tilde); 
                m_positions.segment(first * 3, 3) += (1 / m_particle_mass) * dCj * delta_lambda_j;
                m_positions.segment(second * 3, 3) -= (1 / m_particle_mass) * dCj * delta_lambda_j;
                m_lambda[j] += delta_lambda_j;
            }
        }
        // Volume constraint
        
        if(m_volume_constraint_on) {
            double current_volume = calculate_volume();
            double Cj = current_volume - m_goal_volume;
            if (Cj != 0.0) {

                Eigen::MatrixXd dCj = Eigen::MatrixXd::Zero(1, m_num_elements);
                
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
                double delta_lambda_denom = (dCj * (1.0 / m_particle_mass) * dCj.transpose()).eval().value() + inverse_stiffness_tilde;
                
                if (delta_lambda_denom != 0.0) {
                    // The first m_edges.size() elements of lambda correspond to the distance constraints
                    // The element after those, which is indexed at m_edges.size(), corresponds to the volume constraint.
                    double delta_lambda = (-Cj - inverse_stiffness_tilde * m_lambda(m_edges.size())) / delta_lambda_denom;
                    m_lambda[m_edges.size()] += delta_lambda;
                    m_positions += (1.0 / m_particle_mass) * dCj.transpose() * delta_lambda;
                }
            }
        }

        // Grabbing constraint (Distance constraint of the dragged vertex with mouse position)
        if (m_is_grabbed && m_grabbed_vertex != -1) {
            Eigen::Vector3d vertex_position = m_positions.segment(3 * m_grabbed_vertex, 3);
            Eigen::Vector3d goal_pos = m_drag_pos;

            std::cout << "goal_pos: (" << goal_pos.x() << ", " << goal_pos.y() << ", " << goal_pos.z() << ")" << std::endl;

            double diff_norm = (goal_pos - vertex_position).norm();

            if (diff_norm < 1e-7) continue; // Prevent some weird behaviours because of division

            double Cj = diff_norm - 0; // length = 0
            Eigen::Vector3d dCj = (goal_pos - vertex_position) / diff_norm;

            double delta_lambda_j = Cj / ((1 / m_particle_mass) + 0.01 * dt * dt); 
            m_positions.segment(m_grabbed_vertex * 3, 3) += (1 / m_particle_mass) * dCj * delta_lambda_j;
            // No need to update lambda, as this constraint is separate
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

    // Optional: Apply friction for particles on floor
    for (int i = 0; i < m_num_elements; i += 3) {
        
        if (m_positions[i+1] == 0.0) {
            Eigen::Vector3d v = m_velocities.segment(i, 3);

            m_velocities.segment(i, 3) = Eigen::Vector3d{(1 - m_friction_coefficient) * v.x(), v.y(), (1 - m_friction_coefficient) * v.z()};
        }
    }

    update_vbo();

    //std::cout << "XPBD simulation step time: " << glfwGetTime() - prev_time << std::endl;
}


void SoftbodyXPBD::set_inverse_stiffness(float inv_stiff) {
    m_inverse_stiffness = inv_stiff;
}

void SoftbodyXPBD::set_solver_iterations(unsigned iters) {
    m_solver_iterations = iters;
}

void SoftbodyXPBD::set_distance_constraint_on(bool val) {
    m_distance_constraint_on = val;
}

void SoftbodyXPBD::set_volume_constraint_on(bool val) {
    m_volume_constraint_on = val;
}

double SoftbodyXPBD::calculate_volume() {
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

void SoftbodyXPBD::reset_object() {

    // SIMULATION

    Eigen::Vector3d e_center;
    e_center << m_center.x, m_center.y, m_center.z;

    Eigen::AngleAxisd roll_angle(m_angles.x, Eigen::Vector3d::UnitX());
    Eigen::AngleAxisd pitch_angle(m_angles.y, Eigen::Vector3d::UnitY());
    Eigen::AngleAxisd yaw_angle(m_angles.z, Eigen::Vector3d::UnitZ());

    Eigen::Matrix3d e_rotation = (yaw_angle * pitch_angle * roll_angle).matrix();

    m_positions = m_obj->positions;
    Eigen::Vector3d scale(m_scale.x, m_scale.y, m_scale.z);

    for (int i = 0; i < m_positions.size(); i += 3) {
        m_positions.segment(i, 3).array() *= scale.array();
        m_positions.segment(i, 3) = e_center + e_rotation * m_positions.segment(i, 3);
    }

    m_num_elements = m_positions.size();

    m_velocities = Eigen::VectorXd::Zero(m_num_elements);

    m_gravity = Eigen::VectorXd::Zero(m_num_elements);
    for (size_t i = 1; i < m_num_elements; i += 3) {
        m_gravity(i) = -9.81;
    }

    m_edges = m_obj->edges;
    
    m_edge_indices.reserve(m_edges.size());

    for(size_t i = 0; i < m_edges.size(); ++i) {
        m_edge_indices.push_back(i);
    }
    
    m_triangles = m_obj->triangles;

    m_num_constraints = m_edges.size() + 1; // num_edges distance constraints + 1 volume constraint
    m_lambda = Eigen::VectorXd::Zero(m_num_constraints);

    m_initial_volume = calculate_volume();
    m_goal_volume = m_goal_volume_multiplier * m_initial_volume; // needs to be called after storing triangles and starting positions

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

size_t SoftbodyXPBD::get_number_of_vertices() {
    return m_vertices.size() / 3;
}

size_t SoftbodyXPBD::get_number_of_indices() {
    return m_indices.size();
}

const Eigen::VectorXd& SoftbodyXPBD::get_positions() const {
    return m_positions;
}

void SoftbodyXPBD::grab(int grabbed_vertex, Eigen::Vector3d goal_pos) {
    m_is_grabbed = true;
    m_grabbed_vertex = grabbed_vertex;
    m_drag_pos = goal_pos;
}

void SoftbodyXPBD::update_grab_goal(Eigen::Vector3d goal_pos) {
    m_drag_pos = goal_pos;
}

void SoftbodyXPBD::ungrab() {
    m_is_grabbed = false;
    m_grabbed_vertex = -1;
}