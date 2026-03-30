#include "Renderer.hpp"
#include "Shader.hpp"
#include <iostream>
#include <GLAD/glad.h>
#include "SoftbodyCubeMassSpring.hpp"
#include "Solid.hpp"

Renderer::Renderer() {

    // Shaders created on the stack and deleted after this scope ends. They are stored in a program before they are deleted, after which they are no longer needed.
    Shader basic_vertex_shader(GL_VERTEX_SHADER, "../shaders/vertex/basic.glsl");
    Shader basic_fragment_shader(GL_FRAGMENT_SHADER, "../shaders/fragment/basic.glsl");
    basic_vertex_shader.compile_shader();
    basic_fragment_shader.compile_shader();

    m_basic_shader_program = std::make_unique<ShaderProgram>(basic_vertex_shader, basic_fragment_shader);    
    m_basic_shader_program->link_program();

    Shader phong_vertex_shader(GL_VERTEX_SHADER, "../shaders/vertex/phong.glsl");
    Shader phong_fragment_shader(GL_FRAGMENT_SHADER, "../shaders/fragment/phong.glsl");
    phong_vertex_shader.compile_shader();
    phong_fragment_shader.compile_shader();

    m_phong_shader_program = std::make_unique<ShaderProgram>(phong_vertex_shader, phong_fragment_shader);    
    m_phong_shader_program->link_program();
}

Renderer::~Renderer() {

}

void Renderer::render_object(const Object& shape) {

}

void Renderer::render_mass_spring(Camera* camera, const Scene* scene) {
        
    glClearColor(130.0f/255.0f, 200.0f/255.0f, 229.0f/255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    Solid* platform = scene->get_solid(0);
    GLuint platform_vao = platform->get_vao();

    glUseProgram(m_phong_shader_program->get_id());
    int modelLoc = glGetUniformLocation(m_phong_shader_program->get_id(), "model");
    int viewLoc = glGetUniformLocation(m_phong_shader_program->get_id(), "view");
    int projectionLoc = glGetUniformLocation(m_phong_shader_program->get_id(), "projection");
    int colorLoc = glGetUniformLocation(m_phong_shader_program->get_id(), "color");
    int viewPosLoc = glGetUniformLocation(m_phong_shader_program->get_id(), "viewPos");
    glm::vec3 orange = glm::vec3(1.0f, 0.5f, 0.2f);
    glm::vec3 green = glm::vec3(0.2f, 0.6f, 0.2f);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(platform->get_model()));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->get_view()));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(camera->get_projection()));
    glUniform3fv(colorLoc, 1, glm::value_ptr(orange));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera->get_position()));

    glBindVertexArray(platform_vao);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glUseProgram(m_basic_shader_program->get_id());
    modelLoc = glGetUniformLocation(m_basic_shader_program->get_id(), "model");
    viewLoc = glGetUniformLocation(m_basic_shader_program->get_id(), "view");
    projectionLoc = glGetUniformLocation(m_basic_shader_program->get_id(), "projection");
    colorLoc = glGetUniformLocation(m_basic_shader_program->get_id(), "color");
    
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->get_view()));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(camera->get_projection()));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // "Model matrix" already applied to particle positions
    glUniform3fv(colorLoc, 1, glm::value_ptr(green));

    if(scene->get_current_object() == "cube") {
        //std::cout << "MS CUBE RENDERED" << std::endl;
        SoftbodyMassSpring* sb_cube = scene->get_sb_obj_ms("cube");
        GLuint sb_cube_vao = sb_cube->get_vao();

        glBindVertexArray(sb_cube_vao);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPointSize(10.0f);
        glDrawArrays(GL_POINTS, 0, sb_cube->get_number_of_vertices());
        glDrawElements(GL_LINES, sb_cube->get_number_of_indices(), GL_UNSIGNED_INT, 0);
    }
    else if(scene->get_current_object() == "sphere") {
        //std::cout << "MS SPHERE RENDERED" << std::endl;
        SoftbodyMassSpring* sb_sphere = scene->get_sb_obj_ms("sphere");
        GLuint sb_sphere_vao = sb_sphere->get_vao();

        glBindVertexArray(sb_sphere_vao);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPointSize(10.0f);
        glDrawArrays(GL_POINTS, 0, sb_sphere->get_number_of_vertices());
        glDrawElements(GL_LINES, sb_sphere->get_number_of_indices(), GL_UNSIGNED_INT, 0);
    }
    else if(scene->get_current_object() == "detailed_sphere") {
        //std::cout << "MS DETAILED_SPHERE RENDERED" << std::endl;
        SoftbodyMassSpring* sb_sphere = scene->get_sb_obj_ms("detailed_sphere");
        GLuint sb_sphere_vao = sb_sphere->get_vao();

        glBindVertexArray(sb_sphere_vao);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPointSize(10.0f);
        glDrawArrays(GL_POINTS, 0, sb_sphere->get_number_of_vertices());
        glDrawElements(GL_LINES, sb_sphere->get_number_of_indices(), GL_UNSIGNED_INT, 0);
    }
    else if(scene->get_current_object() == "adaptable_cube") {
        //std::cout << "MS ADAPTABLE_CUBE RENDERED" << std::endl;
        SoftbodyCubeMassSpring* sb_cube = scene->get_sb_cube_ms();
        GLuint sb_cube_vao = sb_cube->get_vao();
        unsigned grid_dim = sb_cube->get_grid_dim();

        glBindVertexArray(sb_cube_vao);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPointSize(10.0f);
        glDrawArrays(GL_POINTS, 0, grid_dim * grid_dim * grid_dim);
        glDrawElements(GL_LINES, 1128, GL_UNSIGNED_INT, 0);
    }
    //glDrawElements(GL_POINTS, 36, GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}

void Renderer::render_XPBD(Camera* camera, const Scene* scene) {
        
    glClearColor(130.0f/255.0f, 200.0f/255.0f, 229.0f/255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    Solid* platform = scene->get_solid(0);
    GLuint platform_vao = platform->get_vao();

    glUseProgram(m_phong_shader_program->get_id());
    int modelLoc = glGetUniformLocation(m_phong_shader_program->get_id(), "model");
    int viewLoc = glGetUniformLocation(m_phong_shader_program->get_id(), "view");
    int projectionLoc = glGetUniformLocation(m_phong_shader_program->get_id(), "projection");
    int colorLoc = glGetUniformLocation(m_phong_shader_program->get_id(), "color");
    int viewPosLoc = glGetUniformLocation(m_phong_shader_program->get_id(), "viewPos");
    glm::vec3 orange = glm::vec3(1.0f, 0.5f, 0.2f);
    glm::vec3 green = glm::vec3(0.2f, 0.6f, 0.2f);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(platform->get_model()));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->get_view()));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(camera->get_projection()));
    glUniform3fv(colorLoc, 1, glm::value_ptr(orange));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera->get_position()));

    glBindVertexArray(platform_vao);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glUseProgram(m_basic_shader_program->get_id());
    modelLoc = glGetUniformLocation(m_basic_shader_program->get_id(), "model");
    viewLoc = glGetUniformLocation(m_basic_shader_program->get_id(), "view");
    projectionLoc = glGetUniformLocation(m_basic_shader_program->get_id(), "projection");
    colorLoc = glGetUniformLocation(m_basic_shader_program->get_id(), "color");
    
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->get_view()));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(camera->get_projection()));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // "Model matrix" already applied to particle positions
    glUniform3fv(colorLoc, 1, glm::value_ptr(green));
    
    if (scene->get_current_object() == "cube") {
        //std::cout << "XPBD CUBE RENDERED" << std::endl;
        SoftbodyXPBD* sb_cube = scene->get_sb_obj_xpbd("cube");
        GLuint sb_cube_vao = sb_cube->get_vao();

        glBindVertexArray(sb_cube_vao);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPointSize(10.0f);
        glDrawArrays(GL_POINTS, 0, sb_cube->get_number_of_vertices());
        glDrawElements(GL_LINES, sb_cube->get_number_of_indices(), GL_UNSIGNED_INT, 0);
    }
    else if (scene->get_current_object() == "sphere") {
        //std::cout << "XPBD SPHERE RENDERED" << std::endl;
        SoftbodyXPBD* sb_obj = scene->get_sb_obj_xpbd("sphere");
        GLuint sb_obj_vao = sb_obj->get_vao();

        glBindVertexArray(sb_obj_vao);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPointSize(10.0f);
        glDrawArrays(GL_POINTS, 0, sb_obj->get_number_of_vertices());
        glDrawElements(GL_LINES, sb_obj->get_number_of_indices(), GL_UNSIGNED_INT, 0);
    }
    else if (scene->get_current_object() == "detailed_sphere") {
        //std::cout << "XPBD DETAILED_SPHERE RENDERED" << std::endl;
        SoftbodyXPBD* sb_obj = scene->get_sb_obj_xpbd("detailed_sphere");
        GLuint sb_obj_vao = sb_obj->get_vao();

        glBindVertexArray(sb_obj_vao);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPointSize(10.0f);
        glDrawArrays(GL_POINTS, 0, sb_obj->get_number_of_vertices());
        glDrawElements(GL_LINES, sb_obj->get_number_of_indices(), GL_UNSIGNED_INT, 0);
    }
}