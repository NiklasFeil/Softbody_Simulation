#include "GUI.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <iostream>

GUI::GUI(GLFWwindow* window, Scene* scene, unsigned current_simulation)
: m_scene(scene), m_current_simulation(current_simulation) {

    std::cout << "Creating GUI object" << std::endl;
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

GUI::~GUI() {
    std::cout << "Destroying GUI object" << std::endl;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUI::start_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
 
    bool p_open = true;
    ImGui::Begin("Simulation", &p_open, ImGuiWindowFlags_MenuBar);

    ImGui::SliderInt("Updates per second", &UPDATES_PER_SECOND, min_ups, max_ups);

    switch (m_current_simulation) {
        case 0:
            if(ImGui::SliderFloat("spring constant linear", &m_spring_constant_linear, min_const, max_const)) {
                m_scene->get_sb_cube_ms()->set_spring_constant_linear(m_spring_constant_linear);
                m_scene->get_sb_obj_ms("cube")->set_spring_constant_linear(m_spring_constant_linear);
                m_scene->get_sb_obj_ms("sphere")->set_spring_constant_linear(m_spring_constant_linear);
                m_scene->get_sb_obj_ms("detailed_sphere")->set_spring_constant_linear(m_spring_constant_linear);
            }
            if(ImGui::SliderFloat("spring constant cubic", &m_spring_constant_cubic, min_const, max_const)) {
                m_scene->get_sb_cube_ms()->set_spring_constant_cubic(m_spring_constant_cubic);
                m_scene->get_sb_obj_ms("cube")->set_spring_constant_cubic(m_spring_constant_cubic);
                m_scene->get_sb_obj_ms("sphere")->set_spring_constant_cubic(m_spring_constant_cubic);
                m_scene->get_sb_obj_ms("detailed_sphere")->set_spring_constant_cubic(m_spring_constant_cubic);
            }
            if(ImGui::SliderFloat("dampening constant", &m_dampening_constant, min_const, max_const)) {
                m_scene->get_sb_cube_ms()->set_dampening_constant(m_dampening_constant);
                m_scene->get_sb_obj_ms("cube")->set_dampening_constant(m_dampening_constant);
                m_scene->get_sb_obj_ms("sphere")->set_dampening_constant(m_dampening_constant);
                m_scene->get_sb_obj_ms("detailed_sphere")->set_dampening_constant(m_dampening_constant);
            }
            /*
            if(ImGui::SliderFloat("penalty constant", &m_penalty_constant, min_penalty, max_penalty)) {
                m_scene->get_sb_cube_ms()->set_penalty_constant(m_penalty_constant);
                m_scene->get_sb_obj_ms("cube")->set_penalty_constant(m_penalty_constant);
                m_scene->get_sb_obj_ms("sphere")->set_penalty_constant(m_penalty_constant);
                m_scene->get_sb_obj_ms("detailed_sphere")->set_penalty_constant(m_penalty_constant);
            }*/
            if(ImGui::SliderFloat("bounce constant", &m_penalty_dampening_constant, min_penalty, max_penalty)) {
                m_scene->get_sb_cube_ms()->set_penalty_dampening_constant(m_penalty_dampening_constant);
                m_scene->get_sb_obj_ms("cube")->set_penalty_dampening_constant(m_penalty_dampening_constant);
                m_scene->get_sb_obj_ms("sphere")->set_penalty_dampening_constant(m_penalty_dampening_constant);
                m_scene->get_sb_obj_ms("detailed_sphere")->set_penalty_dampening_constant(m_penalty_dampening_constant);
            }
            if(ImGui::SliderInt("grid dim", &m_grid_dim, 2, 4)) {
                m_scene->get_sb_cube_ms()->set_grid_dim(m_grid_dim);
            }
            if(ImGui::SliderFloat("friction", &m_friction_coefficient, 0.0, 1.0)) {
                m_scene->get_sb_cube_ms()->set_friction(m_friction_coefficient);
                m_scene->get_sb_obj_ms("cube")->set_friction(m_friction_coefficient);
                m_scene->get_sb_obj_ms("sphere")->set_friction(m_friction_coefficient);
                m_scene->get_sb_obj_ms("detailed_sphere")->set_friction(m_friction_coefficient);
                m_scene->get_sb_obj_xpbd("cube")->set_friction(m_friction_coefficient);
                m_scene->get_sb_obj_xpbd("sphere")->set_friction(m_friction_coefficient);
                m_scene->get_sb_obj_xpbd("detailed_sphere")->set_friction(m_friction_coefficient);
            }
            if(ImGui::SliderFloat("particle mass", &m_particle_mass, min_mass, max_mass)) {
                m_scene->get_sb_cube_ms()->set_particle_mass(m_particle_mass);
                m_scene->get_sb_obj_ms("cube")->set_particle_mass(m_particle_mass);
                m_scene->get_sb_obj_ms("sphere")->set_particle_mass(m_particle_mass);
                m_scene->get_sb_obj_ms("detailed_sphere")->set_particle_mass(m_particle_mass);
                m_scene->get_sb_obj_xpbd("cube")->set_particle_mass(m_particle_mass);
                m_scene->get_sb_obj_xpbd("sphere")->set_particle_mass(m_particle_mass);
                m_scene->get_sb_obj_xpbd("detailed_sphere")->set_particle_mass(m_particle_mass);
            }
            if(ImGui::SliderFloat("gravity multiplier", &m_gravity_multiplier, 0.1, 3)) {
                m_scene->get_sb_cube_ms()->set_gravity_multiplier(m_gravity_multiplier);
                m_scene->get_sb_obj_ms("cube")->set_gravity_multiplier(m_gravity_multiplier);
                m_scene->get_sb_obj_ms("sphere")->set_gravity_multiplier(m_gravity_multiplier);
                m_scene->get_sb_obj_ms("detailed_sphere")->set_gravity_multiplier(m_gravity_multiplier);
                m_scene->get_sb_obj_xpbd("cube")->set_gravity_multiplier(m_gravity_multiplier);
                m_scene->get_sb_obj_xpbd("sphere")->set_gravity_multiplier(m_gravity_multiplier);
                m_scene->get_sb_obj_xpbd("detailed_sphere")->set_gravity_multiplier(m_gravity_multiplier);
            }
            break;        
        case 1:
            if(ImGui::SliderFloat("inverse stiffness", &m_inverse_stiffness, min_const_xpbd, max_const_xpbd)) {
                m_scene->get_sb_obj_xpbd("cube")->set_inverse_stiffness(m_inverse_stiffness);
                m_scene->get_sb_obj_xpbd("sphere")->set_inverse_stiffness(m_inverse_stiffness);
                m_scene->get_sb_obj_xpbd("detailed_sphere")->set_inverse_stiffness(m_inverse_stiffness);
            }
            if(ImGui::SliderInt("solver iterations", &m_solver_iterations, 1, 50)) {
                m_scene->get_sb_obj_xpbd("cube")->set_solver_iterations(m_solver_iterations);
                m_scene->get_sb_obj_xpbd("sphere")->set_solver_iterations(m_solver_iterations);
                m_scene->get_sb_obj_xpbd("detailed_sphere")->set_solver_iterations(m_solver_iterations);
            }
            if(ImGui::SliderFloat("friction", &m_friction_coefficient, 0.0, 1.0)) {
                m_scene->get_sb_cube_ms()->set_friction(m_friction_coefficient);
                m_scene->get_sb_obj_ms("cube")->set_friction(m_friction_coefficient);
                m_scene->get_sb_obj_ms("sphere")->set_friction(m_friction_coefficient);
                m_scene->get_sb_obj_ms("detailed_sphere")->set_friction(m_friction_coefficient);
                m_scene->get_sb_obj_xpbd("cube")->set_friction(m_friction_coefficient);
                m_scene->get_sb_obj_xpbd("sphere")->set_friction(m_friction_coefficient);
                m_scene->get_sb_obj_xpbd("detailed_sphere")->set_friction(m_friction_coefficient);
            }
            if(ImGui::SliderFloat("particle mass", &m_particle_mass, min_mass, max_mass)) {
                m_scene->get_sb_cube_ms()->set_particle_mass(m_particle_mass);
                m_scene->get_sb_obj_ms("cube")->set_particle_mass(m_particle_mass);
                m_scene->get_sb_obj_ms("sphere")->set_particle_mass(m_particle_mass);
                m_scene->get_sb_obj_ms("detailed_sphere")->set_particle_mass(m_particle_mass);
                m_scene->get_sb_obj_xpbd("cube")->set_particle_mass(m_particle_mass);
                m_scene->get_sb_obj_xpbd("sphere")->set_particle_mass(m_particle_mass);
                m_scene->get_sb_obj_xpbd("detailed_sphere")->set_particle_mass(m_particle_mass);
            }
            if(ImGui::SliderFloat("gravity multiplier", &m_gravity_multiplier, 0.0, 3.0)) {
                m_scene->get_sb_cube_ms()->set_gravity_multiplier(m_gravity_multiplier);
                m_scene->get_sb_obj_ms("cube")->set_gravity_multiplier(m_gravity_multiplier);
                m_scene->get_sb_obj_ms("sphere")->set_gravity_multiplier(m_gravity_multiplier);
                m_scene->get_sb_obj_ms("detailed_sphere")->set_gravity_multiplier(m_gravity_multiplier);
                m_scene->get_sb_obj_xpbd("cube")->set_gravity_multiplier(m_gravity_multiplier);
                m_scene->get_sb_obj_xpbd("sphere")->set_gravity_multiplier(m_gravity_multiplier);
                m_scene->get_sb_obj_xpbd("detailed_sphere")->set_gravity_multiplier(m_gravity_multiplier);
            }
            if(ImGui::SliderFloat("goal volume multiplier", &m_goal_volume_multiplier, 0.5, 10)) {
                m_scene->get_sb_obj_xpbd("cube")->set_goal_volume_multiplier(m_goal_volume_multiplier);
                m_scene->get_sb_obj_xpbd("sphere")->set_goal_volume_multiplier(m_goal_volume_multiplier);
                m_scene->get_sb_obj_xpbd("detailed_sphere")->set_goal_volume_multiplier(m_goal_volume_multiplier);
            }
            if(ImGui::Checkbox("Distance Constraint", &m_distance_constraint_on)) {
                m_scene->get_sb_obj_xpbd("cube")->set_distance_constraint_on(m_distance_constraint_on);
                m_scene->get_sb_obj_xpbd("sphere")->set_distance_constraint_on(m_distance_constraint_on);
                m_scene->get_sb_obj_xpbd("detailed_sphere")->set_distance_constraint_on(m_distance_constraint_on);
            }
            if(ImGui::Checkbox("Volume Constraint", &m_volume_constraint_on)) {
                m_scene->get_sb_obj_xpbd("cube")->set_volume_constraint_on(m_volume_constraint_on);
                m_scene->get_sb_obj_xpbd("sphere")->set_volume_constraint_on(m_volume_constraint_on);
                m_scene->get_sb_obj_xpbd("detailed_sphere")->set_volume_constraint_on(m_volume_constraint_on);
            }
            break;
    }


    if(ImGui::Button("Start")) {
        sim_running = true;
    }
    if(ImGui::Button("Step")) {
        run_once = true;
    }
    if(ImGui::Button("Pause")) {
        sim_running = false;
    }
    if(ImGui::Button("Reset")) {
        sim_running = false;
        m_scene->reset();
    }
    if(ImGui::Button("Cube")) {
        sim_running = false;
        m_current_object = "cube";
        m_scene->set_current_object("cube");
        m_scene->reset();
    }
    if(ImGui::Button("Sphere")) {
        sim_running = false;
        m_current_object = "sphere";
        m_scene->set_current_object("sphere");
        m_scene->reset();
    }
    if(ImGui::Button("Detailed Sphere")) {
        sim_running = false;
        m_current_object = "detailed_sphere";
        m_scene->set_current_object("detailed_sphere");
        m_scene->reset();
    }
    if(m_current_simulation == 0) {
        if(ImGui::Button("Adaptable Cube")) {
            sim_running = false;
            m_current_object = "adaptable_cube";
            m_scene->set_current_object("adaptable_cube");
            m_scene->reset();
        }
    }
    if(ImGui::Button("Switch Simulation")) {
        sim_running = false;
        m_current_simulation = (m_current_simulation + 1) % 2;
        m_current_object = "cube";
        m_scene->set_current_object("cube");
        m_scene->set_current_sim(m_current_simulation);
        m_scene->reset();
    }
    ImGui::End();
}

void GUI::render_gui() {
    // Rendering
    // (Your code clears your framebuffer, renders your other stuff etc.)
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // (Your code calls glfwSwapBuffers() etc.)
}

unsigned GUI::get_current_simulation() {
    return m_current_simulation;
}