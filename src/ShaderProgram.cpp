#include <iostream>
#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram(Shader& vertex_shader, Shader& fragment_shader)
    : m_vertex_shader(vertex_shader), m_fragment_shader(fragment_shader) {
    
    m_id = glCreateProgram();
    glAttachShader(m_id, m_vertex_shader.get_id());
    glAttachShader(m_id, m_fragment_shader.get_id());
}

GLboolean ShaderProgram::link_program() {
    
    glLinkProgram(m_id);

    int success;
    char* infoLog;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    
    if(!success) {
        glGetProgramInfoLog(m_id, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADERPROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    return success;
}

ShaderProgram::~ShaderProgram() {
    std::cout << "Shader program " << m_id << " deleted." << std::endl;
    glDeleteProgram(m_id);
}

GLuint ShaderProgram::get_id() {
    return m_id;
}
