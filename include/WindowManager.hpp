#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <memory>

class WindowManager {
  private:
    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> m_window{nullptr, glfwDestroyWindow}; // Stores GLFWwindow and executes glfwDestroyWindow on deletion
    unsigned m_width, m_height;

  public:
    WindowManager();
    ~WindowManager();

    void createWindow(unsigned width, unsigned height, const char* title);
    GLFWwindow* getWindow();
    void setViewport(unsigned x, unsigned y, unsigned width, unsigned height); 
    void loadGLLoader();
    void syncViewportToWindow();
    void desyncViewportToWindow();
};
