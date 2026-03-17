#include <GLAD/glad.h>
#include "Shader.hpp"
#include <iostream>

Shader::Shader() {

}

Shader::Shader(GLuint shader_type, const char* source_code) {
    m_shader_type = shader_type;
    m_source_code = source_code;    
    m_id = glCreateShader(shader_type);
    glShaderSource(m_id, 1, &source_code, NULL);
}

Shader::~Shader() {
    std::cout << "Shader deleted: " << m_id << " of type " << m_shader_type << std::endl;
    glDeleteShader(m_id);
}

GLboolean Shader::compile_shader() {

    // Compile shader
    glCompileShader(m_id);

    // Check if compilation worked
    int success;
    char infoLog[512];
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(m_id, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // success -> GL_TRUE if compiled successfully, else GL_FALSE
    return success;
}

GLuint Shader::get_id() const {
    return m_id;
}

const char* Shader::s_get_source_code(const char* path) {

}