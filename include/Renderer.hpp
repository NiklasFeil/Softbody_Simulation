#pragma once
#include "Object.hpp"
#include "Scene.hpp"
#include "ShaderProgram.hpp"
#include <Camera.hpp>
#include <memory>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Renderer {
    private:
        void render_object(const Object& object);
        std::unique_ptr<ShaderProgram> m_basic_shader_program;

        glm::mat4 m_projection;
        glm::mat4 m_view;
        glm::mat4 m_model;

    public:
        Renderer();
        ~Renderer();
        void render(Camera* camera, const Scene* scene);
};