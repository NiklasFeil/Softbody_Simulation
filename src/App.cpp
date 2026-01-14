#include "App.hpp"
#include "Global.hpp"
#include <iostream>
#include <print>

App::App() {

    m_window_manager = std::make_unique<WindowManager>();
    m_window_manager->createWindow(800, 600, "Softbody Simulation");
    m_window_manager->syncViewportToWindow();
    m_input_manager = std::make_unique<InputManager>();
    m_renderer = std::make_unique<Renderer>();
    m_scene = std::make_unique<Scene>();
}

App::~App() {

}

void App::run() {
    std::cout << "App::run()" << std::endl;

    std::println("Getting window");

    GLFWwindow* window = m_window_manager->getWindow();

    std::println("Adding on press behaviour");

    m_input_manager->add_on_press_behaviour(
        GLFW_KEY_ESCAPE, 
        [&window]() { glfwSetWindowShouldClose(window, true); },
        Global::Input::APPEND
    );

    std::println("Starting while loop");
    while(!glfwWindowShouldClose(window)) {
        // Process input
        m_input_manager->process_input(window);

        // Handle rendering
        m_renderer->render(m_scene.get());
        
        // Check and call events and swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}