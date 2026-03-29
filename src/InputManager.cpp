#include "InputManager.hpp"
#include "Global.hpp"
#include <iostream>

InputManager::InputManager(WindowManager* window_manager) 
: m_window_manager(window_manager) {
    
}

InputManager::~InputManager() {
    m_actions_on_press.clear();
    s_instance = nullptr;
}

InputManager* InputManager::construct_instance(WindowManager* window_manager) {
    std::cout << "Checking InputManager instance..." << std::endl;
    if (s_instance == nullptr) {
        std::cout << "Creating InputManager instance..." << std::endl;
        s_instance = new InputManager(window_manager);
    }
    return s_instance;
}

InputManager* InputManager::get_instance() {
    if (s_instance == nullptr) {
        std::cerr << "When get_instance() of InputManager was called, there was no constructed instance." << std::endl;
    }
    return s_instance;
}

void InputManager::add_on_press_behaviour(int key, std::function<void()> added_behaviour, Global::Input::CallbackBehaviour cb) {
    
    // Check whether key is not in map.
    auto it = m_actions_on_press.find(key);
    if (it == m_actions_on_press.end()) {
        // Key is not in map -> Just add it ignoring callback behaviour
        m_actions_on_press.insert({key, {added_behaviour}});
    }
    else {
        // Key is within map -> Check for callback behaviour
        std::vector<std::function<void()>> current_behaviours = it->second;
        switch (cb) {
            case Global::Input::OVERWRITE:
                m_actions_on_press[key] = {added_behaviour};
                break;
            case Global::Input::PREPEND:
                m_actions_on_press[key].insert(current_behaviours.begin(), added_behaviour);
                break;
            case Global::Input::APPEND:
                m_actions_on_press[key].push_back(added_behaviour);
                break;
            default:
                throw std::runtime_error("Illegitimate callback behaviour!");
        }
    }
}

void InputManager::process_input(GLFWwindow* window) {
    // Walk through map and check whether the button is pressed. If so, execute the actions associated with it.
    for(const auto el: m_actions_on_press) {
        auto key = el.first;
        auto actions = el.second;

        if (glfwGetKey(window, key) == GLFW_PRESS){
            for (const std::function<void()> action : actions) {
                action();
            }
        }
    }

}