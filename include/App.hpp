#pragma once

#include "WindowManager.hpp"
#include "InputManager.hpp"
#include <memory>

class App {
  private:
    std::unique_ptr<WindowManager> m_window_manager; 
    std::unique_ptr<InputManager> m_input_manager;

  public:
    App();
    ~App();

    void run();
};