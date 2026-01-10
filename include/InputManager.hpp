#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include <functional>
#include "Global.hpp"

class InputManager {
    private:
        std::unordered_map<int, std::vector<std::function<void()>>> m_actions_on_press;
    public:
        
        InputManager();
        ~InputManager();
        void add_on_press_behaviour(int key, std::function<void()> added_behaviour, Global::Input::CallbackBehaviour);
        void process_input(GLFWwindow* window);
        
};