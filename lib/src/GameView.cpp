#include "../include/game.hpp"
#include "../include/GameView.hpp"
#include <iostream>
#include <iomanip>

std::string GameView::get_short_name(const std::string& name) {
    std::istringstream iss(name);
    std::string initials;
    std::string word;
    while (iss >> word) {
        if (!word.empty() && std::isalpha(word[0])) {
            initials += word[0];
        }
    }
    return initials;
}

std::string GameView::get_hp(double hp) {
    std::string hp_str = std::to_string(std::round(hp * 100) / 100);
    hp_str.erase(hp_str.find_last_not_of('0') + 1, std::string::npos);
    hp_str.erase(hp_str.find_last_not_of('.') + 1, std::string::npos);
    return hp_str;
}

void GameView::draw_field(Game& game) {
    Matrix<std::string, FIELD_WEIGHT, FIELD_HEIGHT> units;
    units.fill(" ");
    for (const auto& unit : game.teammates()) {
        units.at(unit->y(), unit->x()) = get_short_name(unit->name()) + "(F, " + get_hp(unit->current_HP()) + "HP)";
    }
    for (const auto& unit : game.enemies()) {
        units.at(unit->y(), unit->x()) = get_short_name(unit->name() )+ "(E, " + get_hp(unit->current_HP()) + "HP)";
    }
    std::vector<size_t> column_widths(FIELD_WEIGHT, 0);
    for (size_t j = 0; j < FIELD_WEIGHT; ++j) {
        for (size_t i = 0; i < FIELD_HEIGHT; ++i) {
            size_t cell_length = (game.field().at(j, i).type() == OBSTACLE) ? 1 : units.at(i, j).length();
            column_widths[j] = std::max(column_widths[j], cell_length);
        }
    }
    for (size_t i = 0; i < FIELD_HEIGHT; ++i) {
        for (size_t j = 0; j < FIELD_WEIGHT; ++j) {
            std::cout << "[";
            if (game.field().at(j, i).type() == OBSTACLE) {
                std::cout << std::setw(column_widths[j]) << "X";
            } else {
                std::cout << std::setw(column_widths[j]) << units.at(i, j);
            }
            std::cout << "]";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void GameView::print_menu() {
    std::cout << "1. Summon unit\n";
    std::cout << "2. Print list of the opponent team\n";
    std::cout << "3. Print list of your team\n";
    std::cout << "4. Print your parameters\n";
    std::cout << "5. Accumulate energy\n";
    std::cout << "6. Upgrade school knowledge (50.0 XP for 10%)\n";
    std::cout << "7. Move\n";
    std::cout << "8. Damage enemy\n";
    std::cout << "9. Exit\n\n";
}

void GameView::print_schools(Game& game) {
    std::cout << "Avialable schools:\n\n";
    for (auto& school : game.schools_table()) {
        std::cout << school.first << "\n\n";
    }
}

void GameView::print_skills(Game& game, Summoner& player) {
    for (auto& school : player.characteristics().schools_knowledge) {
        std::cout << "School " << school.first << " (your knowledge is " << school.second << " %):" << "\n\n";
        for (auto& skill : game.schools_table().table()[school.first].skills) {
            std::cout << "Skill: " << skill.name << "\n";
            std::cout << "Unit name: " << skill.characteristics.name << "\n";
            std::cout << "Damage: " << skill.characteristics.damage << "\n";
            std::cout << "Speed: " << skill.characteristics.speed << "\n";
            std::cout << "Amount: " << skill.characteristics.amount << "\n";
            std::cout << "Unit HP: " << skill.characteristics.entity_HP << "\n";
            std::cout << "Required energy: " << skill.required_energy << "\n";
            std::cout << "Minimum knowledge: " << skill.min_knowledge << "\n\n";
        }
    }
}

void GameView::print_team(Game& game, Summoner& player, Team team) {
    std::vector<std::shared_ptr<BaseUnit>>& units = team == ENEMY ? game.enemies() : game.teammates(); 
    for (auto& unit : units) {
            std::cout << "Unit name: " << unit->name() << " ";
            if (unit->name() == player.name()) {
                std::cout << "(you)"; 
            }
            std::cout << "\nDamage: " << unit->damage() << "\n";
            std::cout << "Current HP: " << unit->current_HP() << "\n";
            std::cout << "Amount: " << unit->amount() << "\n";
            std::cout << "Position: x = " << unit->x() << ", y = " << unit->y() << "\n\n";

    }
}

void GameView::print_parameters(Game& game, Summoner& player) {
        std::cout << "HP: " << player.characteristics().current_HP << "\n";
        std::cout << "Energy: " << player.characteristics().current_energy << " / " << player.characteristics().max_energy << "\n";
        std::cout << "Damage: " << player.characteristics().damage << "\n";
        std::cout << "XP: " << player.characteristics().left_XP << "\n";
        std::cout << "x coordinate: " << player.x() << "\n";
        std::cout << "y coordinate: " << player.y() << "\n";
        std::cout << "Accumulation coefficient: " << player.characteristics().accumulation_coefficient << "\n\n";
}
