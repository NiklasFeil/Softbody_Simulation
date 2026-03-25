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

        // XPBD
        const float min_const_xpbd = 0.0f;
        const float max_const_xpbd = 0.2f;
        float m_inverse_stiffness = 0.001f;


        unsigned m_current_simulation; // 0 -> Mass Spring, 1 -> XPBD
        Scene* m_scene;

    public:
        GUI(GLFWwindow* window, Scene* scene, unsigned current_simulation);
        ~GUI();

        void start_frame();
        void render_gui();
        
        int UPDATES_PER_SECOND = 30;
        bool sim_running = false;
        bool run_once = false;
        bool reset = false;

        unsigned get_current_simulation();
};