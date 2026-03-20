#pragma once

#include <glm/vec3.hpp>
#include <GLAD/glad.h>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Sparse>

class SoftbodyCube {
    private:

        int m_num_corners = 8;
        int m_num_elements = 3 * m_num_corners;
        // Positions of vertices used for calculations in simulation. Required because cube is soft and simply transforming cube using model matrix works only with rigid mesh.
        Eigen::VectorXd m_positions = Eigen::VectorXd::Zero(m_num_elements);
        Eigen::VectorXd m_velocities = Eigen::VectorXd::Zero(m_num_elements);

        // Center of cube when spawning. All other movement is stored in m_positions and is relative to m_center
        // => m_center does not change
        glm::vec3 m_center;

        // Data required to construct using VAO
        std::vector<float> m_vertices;
        std::vector<int> m_indices;
        GLuint m_vbo;
        GLuint m_vao;
        GLuint m_ebo;

        void update_vbo();

        Eigen::VectorXd m_gravity = Eigen::VectorXd::Zero(m_num_elements);

        // TODO: Variables required for Mass-Spring System
        // Springs (i, j) from i to j, denoting indices in m_positions
        std::vector<std::tuple<unsigned, unsigned, double>> m_springs; // (particle_i, particle_j, length)
        Eigen::SparseMatrix<double> m_inverse_mass{m_num_elements, m_num_elements};
        double m_spring_constant = 1.0;
        double m_dampening_constant = 1.0;
        double m_penalty_constant = 1.0;
        double m_penalty_dampening_constant = 1.0;

        // TODO: Variables required for XPBD

        
    public:
        SoftbodyCube(std::vector<float> vertices, std::vector<int> indices, glm::vec3 center);
        ~SoftbodyCube();

        GLuint get_vao() const;
        
        void simulate_mass_spring(double dt);
                
        void set_spring_constant(double val) {
            m_spring_constant = val;
        }
        void set_dampening_constant(double val) {
            m_dampening_constant = val;
        }
        void set_penalty_constant(double val) {
            m_penalty_constant = val;
        }
        void set_penalty_dampening_constant(double val) {
            m_penalty_dampening_constant = val;
        }
    };