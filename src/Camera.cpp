#include <Camera.hpp>
#include <InputManager.hpp>

Camera::Camera(glm::vec3 pos, glm::vec3 target)
: m_position(pos), m_target(target) {

    m_phi = 0.0f;
    m_theta = 0.0f;

    float camX = cos(m_theta) * sin(m_phi) * m_radius;
    float camY = sin(m_theta) * m_radius;
    float camZ = cos(m_theta) * cos(m_phi) * m_radius;

    m_position = glm::vec3(camX, camY, camZ);
    m_view = glm::lookAt(m_position, target, glm::vec3(0.0f, 1.0f, 0.0f));

    InputManager* input_manager = InputManager::get_instance();
    
    input_manager->add_on_press_behaviour(GLFW_KEY_A, [&](){
        m_phi -= 0.1;
    }, 
        Global::Input::APPEND
    );

    input_manager->add_on_press_behaviour(GLFW_KEY_D, [&](){
        m_phi += 0.1;
    }, 
        Global::Input::APPEND
    );

    input_manager->add_on_press_behaviour(GLFW_KEY_W, [&](){
        m_theta -= 0.1;
    }, 
        Global::Input::APPEND
    );

    input_manager->add_on_press_behaviour(GLFW_KEY_S, [&](){
        m_theta += 0.1;
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