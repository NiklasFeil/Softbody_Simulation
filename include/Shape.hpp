#pragma once
#include <vector>
#include <GLAD/glad.h>

class Shape {
    private:
        std::vector<float> m_vertices;
        GLuint m_vbo;
        
    public: 
        Shape(std::vector<float> vertices);
        Shape(float* vertices);
        ~Shape();

        GLuint get_vao();
        GLuint m_vao;
        
};