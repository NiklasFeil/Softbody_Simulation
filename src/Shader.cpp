#include <GLAD/glad.h>
#include "Shader.hpp"
#include <iostream>

Shader::Shader() {

}

Shader::Shader(GLuint shader_type, const char* source_code) {
    this->m_shader_type = shader_type;
    this->m_source_code = source_code;    
    this->m_id = glCreateShader(shader_type);
    glShaderSource(this->m_id, 1, &source_code, NULL);
}

Shader::~Shader() {
    std::cout << "Shader deleted: " << m_id << " of type " << m_shader_type << std::endl;
    glDeleteShader(this->m_id);
}

GLboolean Shader::compile_shader() {

    // Compile shader
    glCompileShader(this->m_id);

    // Check if compilation worked
    int success;
    char infoLog[512];
    glGetShaderiv(this->m_id, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(this->m_id, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // success -> GL_TRUE if compiled successfully, else GL_FALSE
    return success;
}

GLuint Shader::get_id() {
    return this->m_id;
}