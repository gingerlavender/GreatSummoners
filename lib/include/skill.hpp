#ifndef SKILL_H
#define SKILL_H

#include "units.hpp"
#include <functional>

struct Skill {
        UnitDescriptor characteristics;
        std::function<std::shared_ptr<BaseUnit>(UnitDescriptor&)> create;
        std::string name;
        double min_knowledge;
        double required_energy;
        double knowledge_coefficient;
        Skill() = default;
        Skill(UnitDescriptor ud, std::function<std::shared_ptr<BaseUnit>(UnitDescriptor&)> f, std::string n, double mk, double nrg, double coef) : characteristics(ud), create(f), name(n), min_knowledge(mk), required_energy(nrg), knowledge_coefficient(coef) {}
};

#endif
