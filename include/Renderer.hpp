#pragma once
#include "Shape.hpp"
#include "Scene.hpp"
#include "ShaderProgram.hpp"
#include <memory>

class Renderer {
    private:
        void render_shape(const Shape& shape);
        
        std::unique_ptr<ShaderProgram> m_basic_shader_program;

    public:
        Renderer();
        ~Renderer();
        void render(const Scene* scene);
        void render_test(GLuint vao, Shape* shape);
};