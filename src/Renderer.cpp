#include "Renderer.hpp"
#include "Shader.hpp"
#include <print>
#include <iostream>
#include <GLAD/glad.h>

Renderer::Renderer() {
    const char* vertex_shader_source = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        " gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

    const char* fragment_shader_source = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";

    // Shaders created on the stack and deleted after this scope ends. They are stored in a program before they are deleted, after which they are no longer needed.
    Shader basic_vertex_shader(GL_VERTEX_SHADER, vertex_shader_source);
    Shader basic_fragment_shader(GL_FRAGMENT_SHADER, fragment_shader_source);
    basic_vertex_shader.compile_shader();
    basic_fragment_shader.compile_shader();

    m_basic_shader_program = std::make_unique<ShaderProgram>(basic_vertex_shader, basic_fragment_shader);    
    m_basic_shader_program->link_program();
}

Renderer::~Renderer() {

}

void Renderer::render_shape(const Shape& shape) {

}

void Renderer::render(const Scene* scene) {
        
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Shape* triangle = scene->get_shape(0);
    GLuint vao = triangle->get_vao();

    glUseProgram(m_basic_shader_program->get_id());
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}