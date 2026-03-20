#include "Renderer.hpp"
#include "Shader.hpp"
#include <print>
#include <iostream>
#include <GLAD/glad.h>
#include "SoftbodyCube.hpp"
#include "Solid.hpp"

Renderer::Renderer() {

    // Shaders created on the stack and deleted after this scope ends. They are stored in a program before they are deleted, after which they are no longer needed.
    Shader basic_vertex_shader(GL_VERTEX_SHADER, "../shaders/vertex/basic.glsl");
    Shader basic_fragment_shader(GL_FRAGMENT_SHADER, "../shaders/fragment/basic.glsl");
    basic_vertex_shader.compile_shader();
    basic_fragment_shader.compile_shader();

    m_basic_shader_program = std::make_unique<ShaderProgram>(basic_vertex_shader, basic_fragment_shader);    
    m_basic_shader_program->link_program();
}

Renderer::~Renderer() {

}

void Renderer::render_object(const Object& shape) {

}

void Renderer::render(Camera* camera, const Scene* scene) {
        
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Solid* platform = scene->get_solid(0);
    GLuint platform_vao = platform->get_vao();

    SoftbodyCube* sb_cube = scene->get_sb_cube();
    GLuint sb_cube_vao = sb_cube->get_vao();
    unsigned grid_dim = sb_cube->get_grid_dim();

    glUseProgram(m_basic_shader_program->get_id());
    int modelLoc = glGetUniformLocation(m_basic_shader_program->get_id(), "model");
    int viewLoc = glGetUniformLocation(m_basic_shader_program->get_id(), "view");
    int projectionLoc = glGetUniformLocation(m_basic_shader_program->get_id(), "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(platform->get_model()));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->get_view()));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(camera->get_projection()));

    glBindVertexArray(platform_vao);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // TODO: Implement a model matrix into softbody cube that considers m_positions and m_vertices
    glBindVertexArray(sb_cube_vao);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, grid_dim * grid_dim * grid_dim);
    //glDrawElements(GL_POINTS, 36, GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}