#ifndef SCHOOL_HPP
#define SCHOOL_HPP

#include "skill.hpp"
#include <compare>

struct School {
    std::string name;
    std::vector<Skill> skills;
    std::vector<std::string> dominant_for;  
    std::strong_ordering operator<=>(const School& school) const {
        if (std::find_if(dominant_for.begin(), dominant_for.end(), [&](auto& school_name) { return school.name == school_name; }) != dominant_for.end()) {
            return std::strong_ordering::greater;
        } else if (std::find_if(school.dominant_for.begin(), school.dominant_for.end(), [&](auto& school_name) { return name == school_name; }) != dominant_for.end()) {
            return std::strong_ordering::less;
        } else {
            return std::strong_ordering::equal;
        }
    }
    School() = default; 
    School(const std::string& n) : name(n), skills(), dominant_for() {}
    School(std::string n, std::vector<Skill>& vec_skills, std::vector<std::string>& vec_relations) : name(n), skills(vec_skills), dominant_for(vec_relations) {}
};

#endif
