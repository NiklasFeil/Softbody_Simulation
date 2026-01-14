#include "Shape.hpp"
#include <print>
#include <iostream>
#include <GLAD/glad.h>

Shape::Shape(std::vector<float> vertices)
    : m_vertices(vertices) {
    
    std::cout << "Called vector constructor of Shape" << std::endl;

    std::cout << "m_vao: " << m_vao << std::endl;
    std::cout << "&m_vao: " << &m_vao << std::endl;
    std::cout << "glGenVertexArrays: " << glGenVertexArrays << std::endl;

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0); // TODO: This will need to be generalized
    glEnableVertexAttribArray(0);

    //glBindVertexArray(0); // Unbind VAO again
  
    std::cout << "End of vector constructor of Shape" << std::endl;

}

Shape::Shape(float* vertices) {
    std::cout << "Called array constructor of Shape" << std::endl;

    std::cout << "m_vao before construction: " << m_vao << std::endl;
    std::cout << "&m_vao before construction: " << &m_vao << std::endl;

    m_vertices.assign(vertices, vertices + (sizeof(vertices) / sizeof(vertices[0])));

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0); // TODO: This will need to be generalized
    glEnableVertexAttribArray(0);

    //glBindVertexArray(0); // Unbind VAO again   

    std::cout << "m_vao after construction: " << m_vao << std::endl;
    std::cout << "&m_vao after construction: " << &m_vao << std::endl;

    std::cout << "End of array constructor of Shape" << std::endl;
}

Shape::~Shape() {

}

GLuint Shape::get_vao() {
    return m_vao;
}