#include "App.hpp"
#include "Global.hpp"

App::App() {

    m_window_manager = std::make_unique<WindowManager>(); 
    m_input_manager = std::make_unique<InputManager>();
}

App::~App() {

}

void App::run() {
    
    m_window_manager->createWindow(800, 600, "Softbody Simulation");
    m_window_manager->loadGLLoader();
    m_window_manager->syncViewportToWindow();

    GLFWwindow* window = m_window_manager->getWindow();

    m_input_manager->add_on_press_behaviour(
        GLFW_KEY_ESCAPE, 
        [&window]() { glfwSetWindowShouldClose(window, true); },
        Global::Input::APPEND
    );
    

    while(!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        glfwPollEvents();
        m_input_manager->process_input(window);
    }

}