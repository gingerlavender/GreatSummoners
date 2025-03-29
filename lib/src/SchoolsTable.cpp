#include "../include/SchoolsTable.hpp"

void SchoolsTable::add_school(const School& school) {
   table()[school.name] = school; 
}

void SchoolsTable::add_skill(const Skill& skill) {
    table()[skill.characteristics.school].skills.push_back(skill);
}

School& SchoolsTable::get_school(const std::string& name) {
    return table()[name];
} 

Skill& SchoolsTable::get_skill(const std::string& school_name, const std::string& skill_name) {
    auto found = std::find_if(get_school(school_name).skills.begin(), get_school(school_name).skills.end(), [&](auto& skill){ return skill.name == skill_name; });
    if (found == get_school(school_name).skills.end()) {
        throw std::invalid_argument("No such skill");
    } else {
        return *found;
    }
}

size_t SchoolsTable::schools_amount() const {
    return table().size();
}

size_t SchoolsTable::skills_amount() const {
    size_t amount = 0;
    for (auto& [name, school] : table()) {
        amount += school.skills.size();
    }
    return amount; 
}
