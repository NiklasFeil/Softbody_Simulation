#include "SoftbodyMassSpring.hpp"
#include <cstring>
#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


SoftbodyMassSpring::SoftbodyMassSpring(Obj* obj, glm::vec3 center, glm::vec3 angles, glm::vec3 scale)
: m_obj(obj), m_center(center), m_angles(angles), m_scale(scale) {
    
    reset_object();
    
}

SoftbodyMassSpring::~SoftbodyMassSpring() {
    m_vertices.clear();
    m_indices.clear();
    m_vertices.shrink_to_fit();
    m_indices.shrink_to_fit();
}

void SoftbodyMassSpring::update_vbo() {
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

GLuint SoftbodyMassSpring::get_vao() const {
    return m_vao;   
};

void SoftbodyMassSpring::simulate(double dt) {
    
    double prev_time = glfwGetTime();
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

        double extension = (position_difference_norm - length);

        double spring_force = m_spring_constant_linear * extension + m_spring_constant_cubic * extension * extension * extension;
        double dampening_force = m_dampening_constant * velocity_difference.dot(direction_ij);

        Eigen::Vector3d force_ij = (spring_force + dampening_force) * direction_ij;
        f.segment(i * 3, 3) += force_ij;
        f.segment(j * 3, 3) += -force_ij;
    }

    // Grabbing mechanic
    if (m_is_grabbed && m_grabbed_vertex != -1) {
        // Simply set position at mouse and set velocity zero (If you do not set it zero, it accumulates velocity infinitely). 
        // Also possible to apply force, but that might be more unstable.
        // Important to do this before applying ground collision, otherwise we might set position to be underground.
       
        m_positions.segment(m_grabbed_vertex * 3, 3) = m_drag_pos;

        m_velocities.segment(m_grabbed_vertex * 3, 3) = Eigen::Vector3d::Zero();
    
    }

    // Penality force for particles in the floor ( y < 0 )
    // Multiple possibilities:
    // 1. Only apply force -> Jittering / Might fall through ground
    // 2. Clamp pos, reflect velocity_y or set 0 -> Bounce, might not be too accurate
    // 3. Clamp pos, apply force -> Bounce in force-based way, no falling through ground
    for (size_t i = 1; i < m_positions.size(); i += 3) {
        double y = m_positions(i);
        double vy = m_velocities(i);
        if (y < 0.0) {

            // Apply force upwards
            f(i) += /*- m_penalty_constant * y */- m_penalty_dampening_constant * vy;

            // Hard constraint
            m_positions[i] = 0.0;

/*            if (vy < 0.0) {
                m_velocities[i] = 0.0; 
                // Instead of applying force, one could also reflect velocity
            }
*/
        }
    }

    // Optional: Apply friction for particles on floor
    for (int i = 0; i < m_num_elements; i += 3) {
        
        if (m_positions[i+1] == 0.0) {
            Eigen::Vector3d v = m_velocities.segment(i, 3);

            Eigen::Vector3d friction = -m_friction_coefficient * Eigen::Vector3d{v.x(), 0.0, v.z()};
            f.segment(i, 3) += friction;
        }
    }

    // Semi-implicit Euler
    Eigen::VectorXd acc = (1.0 / m_particle_mass) * f + m_gravity_multiplier * m_gravity;
    Eigen::VectorXd dv = dt * acc;
    m_velocities += dv;
    Eigen::VectorXd dx = dt * m_velocities;
    m_positions += dx;

    update_vbo();

    std::cout << "Mass Spring simulation step time: " << glfwGetTime() - prev_time << std::endl;
    
}

void SoftbodyMassSpring::reset_object() {

    Eigen::Vector3d e_center;
    e_center << m_center.x, m_center.y, m_center.z;

    Eigen::AngleAxisd roll_angle(m_angles.x, Eigen::Vector3d::UnitX());
    Eigen::AngleAxisd pitch_angle(m_angles.y, Eigen::Vector3d::UnitY());
    Eigen::AngleAxisd yaw_angle(m_angles.z, Eigen::Vector3d::UnitZ());

    Eigen::Matrix3d e_rotation = (yaw_angle * pitch_angle * roll_angle).matrix();

    // Construct m_positions first
    m_positions = m_obj->positions;
    Eigen::Vector3d scale(m_scale.x, m_scale.y, m_scale.z);

    for (int i = 0; i < m_positions.size(); i += 3) {
        m_positions.segment(i, 3).array() *= scale.array();
        m_positions.segment(i, 3) = e_center + e_rotation * m_positions.segment(i, 3);
    }

    m_num_elements = m_positions.size();

    m_velocities = Eigen::VectorXd::Zero(m_num_elements);

    // Construct springs. 
    m_springs = m_obj->edges;

    m_gravity = Eigen::VectorXd::Zero(m_num_elements);
    for (size_t i = 1; i < m_num_elements; i += 3) {
        m_gravity(i) = -9.81;
    }

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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(int), m_indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0); // TODO: This will need to be generalized
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // Unbind VAO again

}

size_t SoftbodyMassSpring::get_number_of_vertices() {
    return m_vertices.size() / 3;
}

size_t SoftbodyMassSpring::get_number_of_indices() {
    return m_indices.size();
}

const Eigen::VectorXd& SoftbodyMassSpring::get_positions() const {
    return m_positions;
}

void SoftbodyMassSpring::change_position(unsigned v, Eigen::Vector3d new_pos) {
    // v is the vertex whose position should change
    m_positions.segment(3 * v, 3) = new_pos;
}

void SoftbodyMassSpring::grab(int grabbed_vertex, Eigen::Vector3d goal_pos) {
    m_is_grabbed = true;
    m_grabbed_vertex = grabbed_vertex;
    m_drag_pos = goal_pos;
}

void SoftbodyMassSpring::update_grab_goal(Eigen::Vector3d goal_pos) {
    m_drag_pos = goal_pos;
}

void SoftbodyMassSpring::ungrab() {
    m_is_grabbed = false;
    m_grabbed_vertex = -1;
}