#include "Grabber.hpp"
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include <glm/vec4.hpp>

Grabber::Grabber(GLFWwindow* window, Camera* camera, Scene* scene)
: m_window(window), m_camera(camera),  m_scene(scene) {
    m_inverse_projection_view = inverse(camera->get_projection() * camera->get_view());
}

Grabber::~Grabber() {

}

/*

OnPress -> Shoot ray, find closest vertex, grab vertex
WhilePressing -> Shoot ray, find point closest to already grabbed vertex
OnRelease -> Ungrab grabbed vertex


*/


void Grabber::check_for_input() {
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !m_mouse_held) {
        // This case should only trigger on the first update when pressing button
        m_mouse_held = true;
        m_grabbed_vertex = -1; // In case it was not properly reset before
        double xpos, ypos;
        glfwGetCursorPos(m_window, &xpos, &ypos);
        //std::cout << "Create ray cast" << std::endl;
        Ray ray = calculate_ray_from_mouse_pos(xpos, ypos);
        std::cout << "Ray: Origin: (" << ray.origin.x() << ", " << ray.origin.y() << ", " << ray.origin.z() << "), Dir: (" << ray.dir.x() << ", " << ray.dir.y() << ", " << ray.dir.z() << ")" << std::endl; 
        //std::cout << "Ray cast succeeded" << std::endl;
        std::string curr_obj = m_scene->get_current_object(); // "cube", "sphere", "detailed_sphere" or "adaptable_cube" (last only for MassSpring)
        unsigned curr_sim = m_scene->get_current_sim(); // 0 -> MassSpring, 1 -> XPBD

        switch (curr_sim) {
            case 0: // MassSpring
                if (curr_obj == "cube" || curr_obj == "sphere" || curr_obj == "detailed_sphere") {
                    SoftbodyMassSpring* softbody = m_scene->get_sb_obj_ms(curr_obj);
                    Eigen::VectorXd positions = softbody->get_positions();
                    // Check what vertex is closest to the ray / player's click
                    double min_dist = 0.3;
                    Eigen::Vector3d new_pos;
                    for (int i = 0; i < positions.size(); i += 3) {
                        Eigen::Vector3d pos = positions.segment(i, 3);
                        auto temp = get_ray_point_distance_and_ray_point(ray, pos);
                        Eigen::Vector3d ray_point = temp.first;
                        double dist = temp.second;
                        //std::cout << "Distance of vertex " << i/3 << ": " << dist << std::endl;
                        if (dist < min_dist) {
                            min_dist = dist;
                            m_grabbed_vertex = i / 3;
                            new_pos = ray_point;
                        }
                    }
                    if (m_grabbed_vertex == -1) {
                        std::cout << "No vertex clicked" << std::endl;
                        return;
                    }
                    std::cout << "Clicked vertex: " << m_grabbed_vertex << std::endl;
                    //softbody->change_position(m_grabbed_vertex, new_pos);
                    softbody->grab(m_grabbed_vertex, new_pos);
                }
                else if (curr_obj == "adaptable_cube") {
                    SoftbodyCubeMassSpring* softbody_cube = m_scene->get_sb_cube_ms();
                    Eigen::VectorXd positions = softbody_cube->get_positions();
                    // Check what vertex is closest to the ray / player's click
                    double min_dist = 0.3;
                    Eigen::Vector3d new_pos;
                    for (int i = 0; i < positions.size(); i += 3) {
                        Eigen::Vector3d pos = positions.segment(i, 3);
                        auto temp = get_ray_point_distance_and_ray_point(ray, pos);
                        Eigen::Vector3d ray_point = temp.first;
                        double dist = temp.second;
                        //std::cout << "Distance of vertex " << i/3 << ": " << dist << std::endl;
                        if (dist < min_dist) {
                            min_dist = dist;
                            m_grabbed_vertex = i / 3;
                            new_pos = ray_point;
                        }
                    }
                    if (m_grabbed_vertex == -1) {
                        std::cout << "No vertex clicked" << std::endl;
                        return;
                    }
                    std::cout << "Clicked vertex: " << m_grabbed_vertex << std::endl;
                    softbody_cube->grab(m_grabbed_vertex, new_pos);
                }
                break;
            case 1: // XPBD
                SoftbodyXPBD* softbody = m_scene->get_sb_obj_xpbd(curr_obj);
                Eigen::VectorXd positions = softbody->get_positions();
                // Check what vertex is closest to the ray / player's click
                double min_dist = 0.3;
                Eigen::Vector3d new_pos;
                for (int i = 0; i < positions.size(); i += 3) {
                    Eigen::Vector3d pos = positions.segment(i, 3);
                    auto temp = get_ray_point_distance_and_ray_point(ray, pos);
                    Eigen::Vector3d ray_point = temp.first;
                    double dist = temp.second;
                    //std::cout << "Distance of vertex " << i/3 << ": " << dist << std::endl; 
                    if (dist < min_dist) {
                        min_dist = dist;
                        m_grabbed_vertex = i / 3; // Closest vertex
                        new_pos = ray_point;
                    }
                }
                if (m_grabbed_vertex == -1) {
                    std::cout << "No vertex clicked" << std::endl;
                    return;
                }
                std::cout << "Clicked vertex: " << m_grabbed_vertex << std::endl;
                softbody->grab(m_grabbed_vertex, new_pos);
                break;
        }
    }
    else if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && m_mouse_held && m_grabbed_vertex != -1) {
        // This case triggers while holding mouse button down
        // Updates the goal position of the grabbed vertex
        double xpos, ypos;
        glfwGetCursorPos(m_window, &xpos, &ypos);
        Ray ray = calculate_ray_from_mouse_pos(xpos, ypos);
        std::cout << "Ray: Origin: (" << ray.origin.x() << ", " << ray.origin.y() << ", " << ray.origin.z() << ")" << std::endl; 
        std::string curr_obj = m_scene->get_current_object(); // "cube", "sphere", "detailed_sphere" or "adaptable_cube" (last only for MassSpring)
        unsigned curr_sim = m_scene->get_current_sim(); // 0 -> MassSpring, 1 -> XPBD

        switch (curr_sim) {
            case 0: // MassSpring
                if (curr_obj == "cube" || curr_obj == "sphere" || curr_obj == "detailed_sphere") {
                    SoftbodyMassSpring* softbody = m_scene->get_sb_obj_ms(curr_obj);
                    Eigen::VectorXd positions = softbody->get_positions();
                    // Find goal position with currently grabbed vertex
                    std::cout << "Grabbed vertex: " << m_grabbed_vertex << std::endl;
                    auto temp = get_ray_point_distance_and_ray_point(ray, positions.segment(3 * m_grabbed_vertex, 3));
                    Eigen::Vector3d new_goal = temp.first;
                    softbody->update_grab_goal(new_goal);
                }
                else if (curr_obj == "adaptable_cube") {
                    SoftbodyCubeMassSpring* softbody_cube = m_scene->get_sb_cube_ms();
                    Eigen::VectorXd positions = softbody_cube->get_positions();
                    // Check what vertex is closest to the ray / player's click
                    auto temp = get_ray_point_distance_and_ray_point(ray, positions.segment(3 * m_grabbed_vertex, 3));
                    Eigen::Vector3d new_goal = temp.first;
                    softbody_cube->update_grab_goal(new_goal);
                }
                break;
            case 1: // XPBD
                SoftbodyXPBD* softbody = m_scene->get_sb_obj_xpbd(curr_obj);
                Eigen::VectorXd positions = softbody->get_positions();
                // Find goal position with currently grabbed vertex
                std::cout << "Grabbed vertex: " << m_grabbed_vertex << std::endl;
                auto temp = get_ray_point_distance_and_ray_point(ray, positions.segment(3 * m_grabbed_vertex, 3));
                Eigen::Vector3d new_goal = temp.first;
                std::cout << "Clicked vertex: " << m_grabbed_vertex << std::endl;
                softbody->update_grab_goal(new_goal);
                break;
        }
    }
    else if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
        m_mouse_held = false;
        m_grabbed_vertex = -1;
        std::string curr_obj = m_scene->get_current_object(); // "cube", "sphere", "detailed_sphere" or "adaptable_cube" (last only for MassSpring)
        unsigned curr_sim = m_scene->get_current_sim(); // 0 -> MassSpring, 1 -> XPBD

        switch (curr_sim) {
            case 0: // MassSpring
                if (curr_obj == "cube" || curr_obj == "sphere" || curr_obj == "detailed_sphere") {
                    SoftbodyMassSpring* softbody = m_scene->get_sb_obj_ms(curr_obj);
                    softbody->ungrab();
                }
                else if (curr_obj == "adaptable_cube") {
                    SoftbodyCubeMassSpring* softbody_cube = m_scene->get_sb_cube_ms();
                    softbody_cube->ungrab();
                }
                break;
            case 1: // XPBD
                SoftbodyXPBD* softbody = m_scene->get_sb_obj_xpbd(curr_obj);
                softbody->ungrab();
                break;
        }
    }
}

Ray Grabber::calculate_ray_from_mouse_pos(double mouse_x, double mouse_y) {
    //std::cout << "Mouse position: (" << mouse_x << ", " << mouse_y << ")" << std::endl;

    float width = 1200.0f, height = 800.0f;

    // Translate mouse positions from screen space to NDC and clip space
    float x = (2.0f * mouse_x) / width - 1.0f;
    float y = 1.0f - (2.0f * mouse_y) / height; // Flip Y

    glm::vec4 ray_start_clip = glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 ray_end_clip = glm::vec4(x, y, 1.0f, 1.0f);

    // Translate start and end of ray to view and then to world space
    m_inverse_projection_view = inverse(m_camera->get_projection() * m_camera->get_view());
    glm::vec4 ray_start_world = m_inverse_projection_view * ray_start_clip;
    glm::vec4 ray_end_world = m_inverse_projection_view * ray_end_clip;

    glm::vec3 ray_start = glm::vec3(ray_start_world.x / ray_start_world.w, 
                                    ray_start_world.y / ray_start_world.w, 
                                    ray_start_world.z / ray_start_world.w);

    glm::vec3 ray_end = glm::vec3(ray_end_world.x / ray_end_world.w, 
                                  ray_end_world.y / ray_end_world.w, 
                                  ray_end_world.z / ray_end_world.w);

    glm::vec3 ray_dir = glm::normalize(ray_end - ray_start);
    
    Ray ray;
    Eigen::Vector3d ray_start_eigen;
    ray_start_eigen << ray_start.x, ray_start.y , ray_start.z;
    Eigen::Vector3d ray_dir_eigen;
    ray_dir_eigen << ray_dir.x, ray_dir.y , ray_dir.z;

    ray.origin = ray_start_eigen;
    ray.dir = ray_dir_eigen;

    return ray;
}

std::pair<Eigen::Vector3d, double> Grabber::get_ray_point_distance_and_ray_point(Ray& ray, Eigen::Vector3d p) {
    Eigen::Vector3d diff = p - ray.origin;
    double t = diff.dot(ray.dir);
    Eigen::Vector3d closest_ray_point = ray.origin + t * ray.dir;
    double dist = (p - closest_ray_point).norm();

    return std::pair(closest_ray_point, dist);    
}