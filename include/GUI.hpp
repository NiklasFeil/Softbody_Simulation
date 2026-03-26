#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Scene.hpp"

class GUI {
    private:

        const int min_ups = 5;
        const int max_ups = 60;

        // Mass Spring System
        const float min_const = 0.1f;
        const float max_const = 10.0f;

        float m_spring_constant = 1.0f;
        float m_dampening_constant = 1.0f;
        float m_penalty_constant = 1.0f;
        float m_penalty_dampening_constant = 1.0f;
        int m_grid_dim = 3;

        // XPBD
        const float min_const_xpbd = 0.0f;
        const float max_const_xpbd = 0.05f;
        float m_inverse_stiffness = 0.001f;
        int m_solver_iterations = 15;
        bool m_distance_constraint_on = true;
        bool m_volume_constraint_on = true;

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