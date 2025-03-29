#ifndef SCHOOLSTABLE_HPP
#define SCHOOLSTABLE_HPP

#include "school.hpp"

class SchoolsTable {
    private:
        std::unordered_map<std::string, School> table_;
    public:
        SchoolsTable() = default;
        SchoolsTable(std::unordered_map<std::string, School>& table) : table_(table) {}
        auto& table() {
            return table_;
        }
        const auto& table() const {
            return table_;
        }
        void add_school(const School& school);
        void add_skill(const Skill& skill);
        Skill& get_skill(const std::string& school_name, const std::string& skill_name);
        School& get_school(const std::string& name);
        size_t skills_amount() const;
        size_t schools_amount() const;
        auto begin() { return table_.begin(); }
        auto cbegin() { return table_.cbegin(); }
        auto end() { return table_.end(); }
        auto cend() { return table_.cend(); }
};

#endif
