#include "App.hpp"

App::App() {

    m_window_manager = std::make_unique<WindowManager>(); 
}

App::~App() {

}

void App::run() {
    
    m_window_manager->createWindow(800, 600, "Softbody Simulation");
    m_window_manager->loadGLLoader();
    m_window_manager->syncViewportToWindow();

    GLFWwindow* window = m_window_manager->getWindow();

    while(!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

}