#pragma once
#include <vector>
#include <memory>
#include "Solid.hpp"
#include "SoftbodyCubeMassSpring.hpp"
#include "SoftbodyMassSpring.hpp"
#include "SoftbodyXPBD.hpp"

class Scene {
    private:
        std::vector<std::unique_ptr<Solid>> m_solids;
        std::unique_ptr<SoftbodyMassSpring> m_sb_cube_ms;
        std::unique_ptr<SoftbodyMassSpring> m_sb_sphere_ms;
        std::unique_ptr<SoftbodyMassSpring> m_sb_detailed_sphere_ms;
        std::unique_ptr<SoftbodyCubeMassSpring> m_sb_adaptable_cube_ms;
        std::unique_ptr<SoftbodyXPBD> m_sb_cube_xpbd;
        std::unique_ptr<SoftbodyXPBD> m_sb_sphere_xpbd;
        std::unique_ptr<SoftbodyXPBD> m_sb_detailed_sphere_xpbd;

        unsigned m_current_simulation;
        std::string m_current_object;

    public:
        Scene(unsigned sim, const char* obj);
        ~Scene();
        Solid* get_solid(size_t idx) const;
        SoftbodyCubeMassSpring* get_sb_cube_ms() const;
        SoftbodyMassSpring* get_sb_obj_ms(std::string obj) const;
        SoftbodyXPBD* get_sb_obj_xpbd(std::string obj) const;
        unsigned get_current_sim() const;
        std::string get_current_object() const;
        void set_current_sim(unsigned sim);
        void set_current_object(std::string obj);
        void reset();
};