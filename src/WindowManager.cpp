#include "WindowManager.hpp"
#include <iostream>
#include <GLAD/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <memory>
#include "Global.hpp"

WindowManager::WindowManager()
  : m_width(0), m_height(0) {

    std::cout << "Initializing GLFW..." << std::endl;
    /* Initialize the library */
    if (!glfwInit())
      throw std::runtime_error("GLFW could not be initialized properly");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSetErrorCallback(Global::print_error_callback);
}

WindowManager::~WindowManager() {
  //m_window = nullptr;
  glfwSetErrorCallback([](int e, const char* c){});
  glfwDestroyWindow(this->m_window.get());
  glfwTerminate();
}

void WindowManager::createWindow(unsigned width, unsigned height, const char* title) {
  /**
   * Creates a new GLFW window or replaces existing GLFW window with the given metrics and stores it in m_window, which can later be extracted using getWindow()
  */

  m_window.reset(
    glfwCreateWindow(width, height, title, NULL, NULL)
  );

  m_width = width;
  m_height = height;

  if (m_window == nullptr) {
    throw std::runtime_error("Failed to create GLFW window");
  }
  
  glfwMakeContextCurrent(m_window.get());

  std::cout << "Initializing GLAD..." << std::endl;

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    throw std::runtime_error("Failed to initialize GLAD");  
  }
}

GLFWwindow* WindowManager::getWindow() {
  return m_window.get();
}

void WindowManager::setViewport(unsigned x, unsigned y, unsigned width, unsigned height) {
  glViewport(x, y, width, height); 
}

void WindowManager::syncViewportToWindow() {
  glViewport(0, 0, m_width, m_height);
  glfwSetFramebufferSizeCallback(m_window.get(), [](GLFWwindow* win, int w, int h) {
    glViewport(0, 0, w, h);
  });
}

void WindowManager::desyncViewportToWindow() {
  glfwSetFramebufferSizeCallback(m_window.get(), [](GLFWwindow* win, int w, int h){});
}
