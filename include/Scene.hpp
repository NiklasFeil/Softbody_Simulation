#pragma once
#include <vector>
#include <memory>
#include "Solid.hpp"
#include "SoftbodyCubeMassSpring.hpp"
#include "SoftbodyCubeXPBD.hpp"

class Scene {
    private:
        std::vector<std::unique_ptr<Solid>> m_solids;
        std::unique_ptr<SoftbodyCubeMassSpring> m_sb_cube_ms;
        std::unique_ptr<SoftbodyCubeXPBD> m_sb_cube_xpbd;

        unsigned m_current_simulation;

    public:
        Scene(unsigned sim);
        ~Scene();
        Solid* get_solid(size_t idx) const;
        SoftbodyCubeMassSpring* get_sb_cube_ms() const;
        SoftbodyCubeXPBD* get_sb_cube_xpbd() const;
        unsigned get_current_sim();
};