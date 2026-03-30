#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "Camera.hpp"
#include "Scene.hpp"

struct Ray {
    Eigen::Vector3d origin;
    Eigen::Vector3d dir;
};

class Grabber {
    private:
        GLFWwindow* m_window;
        Camera* m_camera;
        Scene* m_scene;

        glm::mat4 m_inverse_projection_view;

        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

        std::pair<Eigen::Vector3d, double> get_ray_point_distance_and_ray_point(Ray& ray, Eigen::Vector3d pos);
    
        bool m_mouse_held = false;
        int m_grabbed_vertex = -1;

    public:
        Grabber(GLFWwindow* window, Camera* camera, Scene* scene);
        ~Grabber();

        void check_for_input();
        Ray calculate_ray_from_mouse_pos(double x, double y);
};