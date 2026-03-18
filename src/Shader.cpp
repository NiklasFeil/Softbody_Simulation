#include <GLAD/glad.h>
#include "Shader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

Shader::Shader() {

}

Shader::Shader(GLuint shader_type, const char* path) {
    m_shader_type = shader_type;

    std::ifstream shader_file;
    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        std::cout << "Working Directory: " << std::filesystem::current_path() << std::endl;
        // open files
        shader_file.open(path);
        std::stringstream shader_stream;
        // read file's buffer contents into streams
        shader_stream << shader_file.rdbuf();
        // close file handler
        shader_file.close();
        // convert stream into string
        m_source_code = shader_stream.str();
    }
    catch(std::ifstream::failure& e) {
        std::cout << "ERROR::shader::FILE_NOT_SUCCESSFULLY_READ | Error code: " << e.code() << std::endl;
    }

    const char* source_code_c = m_source_code.c_str();
    m_id = glCreateShader(shader_type);
    glShaderSource(m_id, 1, &source_code_c, NULL);
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

