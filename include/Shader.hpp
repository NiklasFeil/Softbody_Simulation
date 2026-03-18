#pragma once
#include <string>
#include <GLAD/glad.h>

class Shader {
    private:
        std::string m_source_code;
        GLuint m_id;
        GLuint m_shader_type;

    public:
        Shader();
        Shader(GLuint shader_type, const char* source_code);
        ~Shader();

        GLboolean compile_shader();
        GLuint get_id() const;
};