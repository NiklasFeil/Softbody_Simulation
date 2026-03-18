#include "Renderer.hpp"
#include "Shader.hpp"
#include <print>
#include <iostream>
#include <GLAD/glad.h>

Renderer::Renderer() {

    // Shaders created on the stack and deleted after this scope ends. They are stored in a program before they are deleted, after which they are no longer needed.
    Shader basic_vertex_shader(GL_VERTEX_SHADER, "../shaders/vertex/basic.glsl");
    Shader basic_fragment_shader(GL_FRAGMENT_SHADER, "../shaders/fragment/basic.glsl");
    basic_vertex_shader.compile_shader();
    basic_fragment_shader.compile_shader();

    m_basic_shader_program = std::make_unique<ShaderProgram>(basic_vertex_shader, basic_fragment_shader);    
    m_basic_shader_program->link_program();

    m_view = glm::mat4(1.0f);
    m_view = glm::translate(m_view, glm::vec3(0.0f, 0.0f, -3.0f));
    m_model = glm::mat4(1.0f);
    m_model = glm::rotate(m_model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    m_projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
}

Renderer::~Renderer() {

}

void Renderer::render_shape(const Shape& shape) {

}

void Renderer::render(Camera* camera, const Scene* scene) {
        
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Shape* triangle = scene->get_shape(0);
    GLuint vao = triangle->get_vao();

    glUseProgram(m_basic_shader_program->get_id());
    int modelLoc = glGetUniformLocation(m_basic_shader_program->get_id(), "model");
    int viewLoc = glGetUniformLocation(m_basic_shader_program->get_id(), "view");
    int projectionLoc = glGetUniformLocation(m_basic_shader_program->get_id(), "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->get_view()));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(m_projection));

    glBindVertexArray(vao);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}