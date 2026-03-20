#pragma once

#include <vector>
#include <GLAD/glad.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Solid {
    private:

        glm::vec3 m_position;
        glm::vec3 m_angles;
        glm::vec3 m_scale;

        glm::mat4 m_model;

        std::vector<float> m_vertices;
        std::vector<int> m_indices;
        GLuint m_vbo;
        GLuint m_vao;
        GLuint m_ebo;
        
    public: 
        Solid(std::vector<float> vertices, std::vector<int> indices, glm::vec3 position, glm::vec3 euler_angles, glm::vec3 scale);
        Solid(float* vertices, int* indices);
        ~Solid();

        GLuint get_vao();
        glm::mat4 get_model();
};