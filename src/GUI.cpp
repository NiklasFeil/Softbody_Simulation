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
 
    // Create a window called "My First Tool", with a menu bar.
    bool p_open = true;
    ImGui::Begin("Simulation", &p_open, ImGuiWindowFlags_MenuBar);

    ImGui::SliderInt("Updates per second", &UPDATES_PER_SECOND, min_ups, max_ups);

    switch (m_current_simulation) {
        case 0:
            if(ImGui::SliderFloat("spring constant", &m_spring_constant, min_const, max_const)) {
                m_scene->get_sb_cube_ms()->set_spring_constant(m_spring_constant);
            }
            if(ImGui::SliderFloat("dampening constant", &m_dampening_constant, min_const, max_const)) {
                m_scene->get_sb_cube_ms()->set_dampening_constant(m_dampening_constant);
            }
            if(ImGui::SliderFloat("penalty constant", &m_penalty_constant, min_const, max_const)) {
                m_scene->get_sb_cube_ms()->set_penalty_constant(m_penalty_constant);
            }
            if(ImGui::SliderFloat("penalty dampening constant", &m_penalty_dampening_constant, min_const, max_const)) {
                m_scene->get_sb_cube_ms()->set_penalty_dampening_constant(m_penalty_dampening_constant);
            }
            if(ImGui::SliderInt("grid dim", &m_grid_dim, 2, 4)) {
                m_scene->get_sb_cube_ms()->set_grid_dim(m_grid_dim);
            }
            break;        
        case 1:
            if(ImGui::SliderFloat("inverse stiffness", &m_inverse_stiffness, min_const_xpbd, max_const_xpbd)) {
                m_scene->get_sb_obj_xpbd(m_current_object)->set_inverse_stiffness(m_inverse_stiffness);
            }
            if(ImGui::SliderInt("solver iterations", &m_solver_iterations, 1, 50)) {
                m_scene->get_sb_obj_xpbd(m_current_object)->set_solver_iterations(m_solver_iterations);
            }
            if(ImGui::Checkbox("Distance Constraint", &m_distance_constraint_on)) {
                m_scene->get_sb_obj_xpbd(m_current_object)->set_distance_constraint_on(m_distance_constraint_on);
            }
            if(ImGui::Checkbox("Volume Constraint", &m_volume_constraint_on)) {
                m_scene->get_sb_obj_xpbd(m_current_object)->set_volume_constraint_on(m_volume_constraint_on);
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
    if(ImGui::Button("Switch Simulation")) {
        sim_running = false;
        m_current_simulation = (m_current_simulation + 1) % 2;
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