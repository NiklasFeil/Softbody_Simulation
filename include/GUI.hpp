#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Scene.hpp"

class GUI {
    private:

        const int min_ups = 5;
        const int max_ups = 120;

        // Mass Spring System
        const float min_const = 1.0f;
        const float max_const = 150.0f;
        const float min_penalty = 1.0f;
        const float max_penalty = 150.0f;

        float m_spring_constant_linear = 5.0f;
        float m_spring_constant_cubic = 5.0f;
        float m_dampening_constant = 5.0f;
        float m_penalty_constant = 5.0f;
        float m_penalty_dampening_constant = 5.0f;
        int m_grid_dim = 3;

        // XPBD
        const float min_const_xpbd = 0.0f;
        const float max_const_xpbd = 0.01f;
        float m_inverse_stiffness = 0.001f;
        int m_solver_iterations = 5;
        float m_goal_volume_multiplier = 1.0;
        bool m_distance_constraint_on = true;
        bool m_volume_constraint_on = true;

        float min_mass = 0.1, max_mass = 10;
        float m_particle_mass = 1.0;
        float m_gravity_multiplier = 1.0;
        float m_friction_coefficient = 0.5;
        unsigned m_current_simulation; // 0 -> Mass Spring, 1 -> XPBD
        std::string m_current_object = "cube";
        Scene* m_scene;

    public:
        GUI(GLFWwindow* window, Scene* scene, unsigned current_simulation);
        ~GUI();

        void start_frame();
        void render_gui();
        
        int UPDATES_PER_SECOND = 30;
        bool sim_running = false;
        bool run_once = false;

        unsigned get_current_simulation();
};