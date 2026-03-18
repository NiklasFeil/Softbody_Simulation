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
    m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f));
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

    std::println("Starting while loop");
    while(!glfwWindowShouldClose(window)) {
        // Process input
        m_input_manager->process_input(window);

        // Update camera
        m_camera->update();

        // Handle rendering
        m_renderer->render(m_camera.get(), m_scene.get());
        
        // Check and call events and swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}