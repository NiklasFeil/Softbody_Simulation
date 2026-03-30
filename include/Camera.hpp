#pragma once
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <glm/ext.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Camera {

    private:
        glm::vec3 m_position;
        glm::vec3 m_target; 
        glm::vec3 m_front;
        glm::vec3 m_up; 

        float speed;
        float m_roll;
        float m_pitch;
        float m_yaw;

        glm::mat4 m_view;
        glm::mat4 m_projection;

        float m_phi;
        float m_theta;
        float m_radius = 10.0f;

    public:
        Camera();
        ~Camera();

        glm::mat4 get_view();
        glm::mat4 get_projection();
        glm::vec3 get_position();
        void update();


};