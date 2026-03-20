#pragma once
#include <vector>
#include <memory>
#include "Solid.hpp"
#include "SoftbodyCube.hpp"

class Scene {
    private:
        std::vector<std::unique_ptr<Solid>> m_solids;
        std::unique_ptr<SoftbodyCube> m_sb_cube;

    public:
        Scene();
        ~Scene();
        Solid* get_solid(size_t idx) const;
        SoftbodyCube* get_sb_cube() const;
};