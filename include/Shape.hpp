#pragma once
#include <vector>
#include <GLAD/glad.h>

class Shape {
    private:
        std::vector<float> m_vertices;
        std::vector<int> m_indices;
        GLuint m_vbo;
        GLuint m_vao;
        GLuint m_ebo;
        
    public: 
        Shape(std::vector<float> vertices, std::vector<int> indices);
        Shape(float* vertices, int* indices);
        ~Shape();

        GLuint get_vao();
};