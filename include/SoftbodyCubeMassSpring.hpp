#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <GLAD/glad.h>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Sparse>

class SoftbodyCubeMassSpring {
    private:
        
        // Positions of vertices used for calculations in simulation. Required because cube is soft and simply transforming cube using model matrix works only with rigid mesh.
        Eigen::VectorXd m_positions;
        Eigen::VectorXd m_velocities;

        // Center of cube when spawning. All other movement is stored in m_positions and is relative to m_center
        // => m_center does not change
        glm::vec3 m_center;
        double m_size;
        glm::vec3 m_angles;

        // Data required to construct using VAO
        std::vector<float> m_vertices;
        std::vector<int> m_indices;
        GLuint m_vbo;
        GLuint m_vao;
        GLuint m_ebo;

        void update_vbo();

        Eigen::VectorXd m_gravity;
        double m_gravity_multiplier = 1.0;
        double m_particle_mass = 1.0;

        // Variables required for Mass-Spring System
        unsigned m_grid_dim;
        int m_num_elements;
        // Springs (i, j) from i to j, denoting indices in m_positions
        std::vector<std::tuple<unsigned, unsigned, double>> m_springs; // (particle_i, particle_j, length)
        double m_spring_constant_linear = 1.0;
        double m_spring_constant_cubic = 1.0;
        double m_dampening_constant = 1.0;
        double m_penalty_constant = 1.0;
        double m_penalty_dampening_constant = 1.0;

        Eigen::VectorXd m_force;

        size_t get_index(size_t i, size_t j, size_t k);
        
    public:
        SoftbodyCubeMassSpring(unsigned grid_dim, glm::vec3 center, glm::vec3 angles, double size);
        ~SoftbodyCubeMassSpring();

        GLuint get_vao() const;

        unsigned get_grid_dim();
        
        void simulate(double dt);
                
        void set_spring_constant_linear(double val) {
            m_spring_constant_linear = val;
        }      
        void set_spring_constant_cubic(double val) {
            m_spring_constant_cubic = val;
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
        void set_particle_mass(double val) {
            m_particle_mass = val;
        }
        void set_gravity_multiplier(double val) {
            m_gravity_multiplier = val;
        }   
        void set_grid_dim(int val) {
            m_grid_dim = val;
            reset_cube();
        }
        void reset_cube();
    };