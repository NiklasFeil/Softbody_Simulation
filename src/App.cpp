#include "App.hpp"
#include "Global.hpp"
#include <iostream>
#include <print>

App::App() {

    m_window_manager = std::make_unique<WindowManager>();
    m_window_manager->createWindow(800, 600, "Softbody Simulation");
    m_window_manager->syncViewportToWindow();
    m_input_manager = InputManager::construct_instance(m_window_manager.get());
    m_renderer = std::make_unique<Renderer>();
    m_scene = std::make_unique<Scene>();
    m_camera = std::make_unique<Camera>();
    m_gui = std::make_unique<GUI>(m_window_manager->getWindow(), m_scene.get());
}

App::~App() {

}

void App::run() {   

    GLFWwindow* window = m_window_manager->getWindow();

    m_input_manager->add_on_press_behaviour(
        GLFW_KEY_ESCAPE, 
        [&window]() { glfwSetWindowShouldClose(window, true); },
        Global::Input::APPEND
    );

    double prev_time = glfwGetTime();
    double curr_time;
    double down_time = 0.0f;;

    std::println("Starting while loop");
    while(!glfwWindowShouldClose(window)) {

        m_gui->start_frame();

        curr_time = glfwGetTime();

        if (m_gui->sim_running || m_gui->run_once) {
            down_time += curr_time - prev_time;
        }

        if(m_gui->reset) {
            m_scene = std::make_unique<Scene>();
            m_gui->reset = false;
        }

        prev_time = curr_time;

        // Process input
        m_input_manager->process_input(window);

        // Simulation
        double dt = 1. / m_gui->UPDATES_PER_SECOND;
        if (down_time >= dt) {
            m_scene->get_sb_cube()->simulate_mass_spring(dt);
            down_time = 0.0f;
            m_gui->run_once = false;
        }

        // Update camera
        m_camera->update();

        // Handle rendering
        m_renderer->render(m_camera.get(), m_scene.get());
        
        // Render GUI
        m_gui->render_gui();

        // Check and call events and swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}