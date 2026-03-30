#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <GLAD/glad.h>
#include <vector>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include "ObjLoader.hpp"

class SoftbodyXPBD {
    private:
        
        // Positions of vertices used for calculations in simulation. Required because cube is soft and simply transforming cube using model matrix works only with rigid mesh.
        Eigen::VectorXd m_positions;
        Eigen::VectorXd m_velocities;
        Eigen::VectorXd m_lambda;
        Obj* m_obj;

        // Center of cube when spawning. All other movement is stored in m_positions and is relative to m_center
        // => m_center does not change
        glm::vec3 m_center;
        glm::vec3 m_angles;
        glm::vec3 m_scale;

        // Data required to construct using VAO
        std::vector<float> m_vertices;
        std::vector<int> m_indices;
        GLuint m_vbo;
        GLuint m_vao;
        GLuint m_ebo;

        void update_vbo();

        Eigen::VectorXd m_gravity;
        double m_particle_mass = 1.0;
        double m_gravity_multiplier = 1.0;

        int m_num_elements;
        int m_num_constraints;
        unsigned m_grid_dim = 2;
        float m_inverse_stiffness = 0.001;
        unsigned m_solver_iterations = 5;
        std::vector<std::tuple<unsigned, unsigned, double>> m_edges; // (particle_i, particle_j, length)
        std::vector<std::tuple<unsigned, unsigned, unsigned>> m_triangles; // (particle_i, particle_j, particle_k)
        std::vector<size_t> m_edge_indices;

        bool m_distance_constraint_on = true;
        bool m_volume_constraint_on = true;

        double m_initial_volume;
        double m_goal_volume;
        double m_goal_volume_multiplier = 1.0; // initial * mult = goal

        size_t get_index(size_t i, size_t j, size_t k);
        double calculate_volume();

        bool m_is_grabbed = false;
        int m_grabbed_vertex = -1; // No vertex grabbed
        Eigen::Vector3d m_drag_pos; // Position where mouse is dragging the grabbed vertex
        
    public:
        SoftbodyXPBD(Obj* obj, glm::vec3 center, glm::vec3 angles, glm::vec3 scale);
        ~SoftbodyXPBD();

        GLuint get_vao() const;
        
        void simulate(double dt);

        unsigned get_grid_dim();

        void set_inverse_stiffness(float inv_stiff);

        void set_solver_iterations(unsigned iters);

        void set_distance_constraint_on(bool val);

        void set_volume_constraint_on(bool val);

        void set_particle_mass(double val) {
            m_particle_mass = val;
        }

        void set_gravity_multiplier(double val) {
            m_gravity_multiplier = val;
        }   
        
        void set_goal_volume_multiplier(double val) {
            m_goal_volume_multiplier = val;
            m_goal_volume = m_initial_volume * m_goal_volume_multiplier;
        }   

        void reset_object();

        size_t get_number_of_vertices();

        size_t get_number_of_indices();

        const Eigen::VectorXd& get_positions() const;

        void grab(int grabbed_vertex, Eigen::Vector3d goal_pos);

        void update_grab_goal(Eigen::Vector3d goal_pos);

        void ungrab();
    };