#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "App.hpp"
#include "WindowManager.hpp"

//void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main() {

  App* app = new App();

  app->run();

  delete app;

  return 0;
}