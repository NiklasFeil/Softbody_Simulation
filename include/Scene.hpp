#pragma once
#include <vector>
#include <memory>
#include "Shape.hpp"

class Scene {
    private:
        std::vector<std::unique_ptr<Shape>> m_shapes;

    public:
        Scene();
        ~Scene();
        Shape* get_shape(size_t idx) const;
};