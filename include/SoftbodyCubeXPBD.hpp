#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <GLAD/glad.h>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Sparse>

class SoftbodyCubeXPBD {
    private:
        
        // Positions of vertices used for calculations in simulation. Required because cube is soft and simply transforming cube using model matrix works only with rigid mesh.
        Eigen::VectorXd m_positions;
        Eigen::VectorXd m_velocities;
        Eigen::VectorXd m_lambda;

        // Center of cube when spawning. All other movement is stored in m_positions and is relative to m_center
        // => m_center does not change
        glm::vec3 m_center;
        double m_size;

        // Data required to construct using VAO
        std::vector<float> m_vertices;
        std::vector<int> m_indices;
        GLuint m_vbo;
        GLuint m_vao;
        GLuint m_ebo;

        void update_vbo();

        Eigen::VectorXd m_gravity;
        Eigen::SparseMatrix<double> m_inverse_mass;

        int m_num_elements;
        int m_num_constraints;
        unsigned m_grid_dim = 2;
        float m_inverse_stiffness = 0.001;
        unsigned m_solver_iterations = 5;
        std::vector<std::tuple<unsigned, unsigned, double>> m_edges; // (particle_i, particle_j, length)
        std::vector<std::tuple<unsigned, unsigned, unsigned>> m_triangles; // (particle_i, particle_j, particle_k)

        double m_goal_volume;

        size_t get_index(size_t i, size_t j, size_t k);
        double calculate_volume();
        
    public:
        SoftbodyCubeXPBD(glm::vec3 center, glm::vec3 angles, double size);
        ~SoftbodyCubeXPBD();

        GLuint get_vao() const;
        
        void simulate(double dt);

        unsigned get_grid_dim();

        void set_inverse_stiffness(float inv_stiff);

        void set_solver_iterations(unsigned iters);
    };