#ifndef DESCRIPTORS_HPP
#define DESCRIPTORS_HPP

#define MAX_MORALITY 1.0
#define MIN_MORALITY -1.0

#include <string>
#include <optional>
#include <unordered_map>

enum Team {
    PLAYER,
    ENEMY
};

struct SummonerDescriptor {
    Team team;
    std::string name;
    double initiative;
    double damage;
    double max_HP;
    double current_HP;
    double accumulation_coefficient;
    double left_XP;
    double max_energy;
    double current_energy;
    std::unordered_map<std::string, double> schools_knowledge;
    SummonerDescriptor(Team init_team, std::string init_name, double init_initiative, double init_damage, double init_max_HP, double init_accumulation_coefficient, double init_max_energy, const std::unordered_map<std::string, double>& init_schools_knowledge) : team(init_team), name(init_name), initiative(init_initiative), damage(init_damage), max_HP(init_max_HP), current_HP(init_max_HP), accumulation_coefficient(init_accumulation_coefficient), left_XP(0), max_energy(init_max_energy), current_energy(init_max_energy), schools_knowledge(init_schools_knowledge) {}
};

struct UnitDescriptor {
    std::string name;
    std::string school;
    int max_amount;
    int amount;
    double initiative;
    double damage;
    double entity_HP;
    double current_HP;
    int speed;
    double defence;
    double xp_for_destroy;
    std::optional<double> morality;
    UnitDescriptor(std::string init_name, std::string init_school, double init_initiative, int init_max_amount, double init_damage, double init_entity_HP, int init_speed, double init_defence, double init_xp_for_destroy, std::optional<double> init_morality) : name(init_name), school(init_school), initiative(init_initiative), max_amount(init_max_amount), amount(init_max_amount), damage(init_damage), entity_HP(init_entity_HP), current_HP(init_entity_HP * init_max_amount), speed(init_speed), defence(init_defence), xp_for_destroy(init_xp_for_destroy), morality(init_morality) {}
    UnitDescriptor() = default;

};

#endif
