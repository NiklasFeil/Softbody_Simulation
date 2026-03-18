#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>
#include <functional>
#include "Global.hpp"
#include "WindowManager.hpp"

class InputManager {
    private:
        // Constructor and Destructor private to prevent creation and detstruction using new and delete
        InputManager(WindowManager* window_manager);
        ~InputManager();

        inline static InputManager* s_instance = nullptr;

        WindowManager* m_window_manager;
        std::unordered_map<int, std::vector<std::function<void()>>> m_actions_on_press;
    public:

        // Not clonable
        InputManager(InputManager &other) = delete;
        // Not assignable
        void operator=(const InputManager&) = delete;

        static InputManager* construct_instance(WindowManager* window_manager);
        static InputManager* get_instance();

        void add_on_press_behaviour(int key, std::function<void()> added_behaviour, Global::Input::CallbackBehaviour);
        void process_input(GLFWwindow* window);
        
};