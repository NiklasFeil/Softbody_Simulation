#include <Camera.hpp>
#include <InputManager.hpp>
#include <cstdint>
#include <iomanip>
#include <iostream>

Camera::Camera() {

    m_phi = 0.0f;
    m_theta = 0.0f;

    float camX = cos(m_theta) * sin(m_phi) * m_radius;
    float camY = sin(m_theta) * m_radius;
    float camZ = cos(m_theta) * cos(m_phi) * m_radius;

    m_position = glm::vec3(camX, camY, camZ);
    m_view = glm::lookAt(m_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    m_projection = glm::perspective(glm::radians(45.0f), 1200.0f / 800.0f, 0.1f, 100.0f);

    InputManager* input_manager = InputManager::get_instance();
    
    input_manager->add_on_press_behaviour(GLFW_KEY_A, [&](){
        m_phi -= 0.075;
    }, 
        Global::Input::APPEND
    );

    input_manager->add_on_press_behaviour(GLFW_KEY_D, [&](){
        m_phi += 0.075;
    }, 
        Global::Input::APPEND
    );

    input_manager->add_on_press_behaviour(GLFW_KEY_W, [&](){
        m_theta += 0.075;
        float pi = glm::pi<float>();
        m_theta = std::min(pi/3.0f, std::max(-pi/3.0f, m_theta));
    }, 
        Global::Input::APPEND
    );

    input_manager->add_on_press_behaviour(GLFW_KEY_S, [&](){
        m_theta -= 0.075;
        float pi = glm::pi<float>();
        m_theta = std::min(pi/3.0f, std::max(-pi/3.0f, m_theta));
    }, 
        Global::Input::APPEND
    );

    input_manager->add_on_press_behaviour(GLFW_KEY_Q, [&](){
        m_radius -= 0.15;
    }, 
        Global::Input::APPEND
    );

    input_manager->add_on_press_behaviour(GLFW_KEY_E, [&](){
        m_radius += 0.15;
    }, 
        Global::Input::APPEND
    );
}

Camera::~Camera() {

}

void Camera::update() {
    float camX = cos(m_theta) * sin(m_phi) * m_radius;
    float camY = sin(m_theta) * m_radius;
    float camZ = cos(m_theta) * cos(m_phi) * m_radius;

    m_position = glm::vec3(camX, camY, camZ);

    m_view = glm::lookAt(m_position, m_target, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::get_view() {
    return m_view;
}

glm::mat4 Camera::get_projection() {
    return m_projection;
}