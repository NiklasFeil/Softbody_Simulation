#pragma once
#include "Shader.hpp"
#include <GLAD/glad.h>

class ShaderProgram {
    private:
        // References need to instantly be initialized in constructor's initializer list
        Shader& m_vertex_shader;
        Shader& m_fragment_shader;
        GLuint m_id;

    public:
        ShaderProgram(Shader& vertex_shader, Shader& fragment_shader);
        ~ShaderProgram();

        GLboolean link_program();
        GLuint get_id();
};