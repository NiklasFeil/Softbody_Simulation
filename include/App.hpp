#pragma once

#include "WindowManager.hpp"
#include "InputManager.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "GUI.hpp"
#include <memory>

class App {
  private:
    std::unique_ptr<WindowManager> m_window_manager; 
    InputManager* m_input_manager;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<GUI> m_gui;

  public:
    App();
    ~App();

    void run();
};